#include <cstdio>
#include <cassert>
#include "node.h"
#include "nodeManager.h"
#include "intersect.h"
#include "bboxGL.h"
#include "renderState.h"
#include "scene.h"

using std::string;
using std::list;

// Recipe 1: iterate through children:
//
//    for(auto & theChild : m_children) {
//        theChild->print(); // or any other thing
//    }

Node::Node(const string &name) :
	m_name(name),
	m_parent(0),
	m_gObject(0),
	m_shader(0),
	m_placement(new Trfm3D),
	m_placementWC(new Trfm3D),
	m_containerWC(new BBox),
	m_checkCollision(true),
	m_isCulled(false),
	m_drawBBox(false) {}

Node::~Node() {
	delete m_placement;
	delete m_placementWC;
	delete m_containerWC;
}

static string name_clone(const string & base) {

	static char MG_SC_BUFF[2048];
	Node *aux;

	int i;
	for(i = 1; i < 1000; i++) {
		sprintf(MG_SC_BUFF, "%s#%d", base.c_str(), i);
		string newname(MG_SC_BUFF);
		aux = NodeManager::instance()->find(newname);
		if(!aux) return newname;
	}
	fprintf(stderr, "[E] Node: too many clones of %s\n.", base.c_str());
	exit(1);
	return string();
}

Node* Node::cloneParent(Node *theParent) {

	string newName = name_clone(m_name);
	Node *newNode = NodeManager::instance()->create(newName);
	newNode->m_gObject = m_gObject;
	newNode->m_lights = m_lights;
	newNode->m_shader = m_shader;
	newNode->m_placement->clone(m_placement);
	newNode->m_parent = theParent;
	for(auto & theChild : m_children) {
		newNode->m_children.push_back(theChild->cloneParent(this));
	}
	return newNode;
}


Node *Node::clone() {
	return cloneParent(0);
}

///////////////////////////////////
// transformations

void Node::attachGobject(GObject *gobj ) {
	if (!gobj) {
		fprintf(stderr, "[E] attachGobject: no gObject for node %s\n", m_name.c_str());
		exit(1);
	}
	if (m_children.size()) {
		fprintf(stderr, "EW] Node::attachGobject: can not attach a gObject to node (%s), which already has children.\n", m_name.c_str());
		exit(1);
	}
	m_gObject = gobj;
	propagateBBRoot();
}

GObject *Node::detachGobject() {
	GObject *res = m_gObject;
	m_gObject = 0;
	return res;
}

GObject *Node::getGobject() {
	return m_gObject;
}

void Node::attachLight(Light *theLight) {
	if (!theLight) {
		fprintf(stderr, "[E] attachLight: no light for node %s\n", m_name.c_str());
		exit(1);
	}
	m_lights.push_back(theLight);
}

int Node::detachLight(Light *theLight) {
	std::vector<Light *> newLights;
	int n = 0;
	for (auto it : m_lights) {
		if (it == theLight) {
			n++;
			continue;
		}
		newLights.push_back(theLight);
	}
	m_lights.swap(newLights);
	return n;
}

void Node::attachShader(ShaderProgram *theShader) {
	if (!theShader) {
		fprintf(stderr, "[E] attachShader: empty shader for node %s\n", m_name.c_str());
		exit(1);
	}
	m_shader = theShader;
}

ShaderProgram *Node::detachShader() {
	ShaderProgram *theShader = m_shader;
	m_shader = 0;
	return theShader;
}

ShaderProgram *Node::getShader() {
	return m_shader;
}

void Node::setDrawBBox(bool b) { m_drawBBox = b; }
bool Node::getDrawBBox() const { return m_drawBBox; }

bool Node::isCulled() const { return m_isCulled; }

void Node::setCheckCollision(bool b) {
	m_checkCollision = b;
	for(auto & theChild : m_children) {
		theChild->setCheckCollision(b);
	}
}

bool Node::getCheckCollision() const { return m_checkCollision; }
const std::string & Node::getName() const {return m_name; }

///////////////////////////////////
// transformations

void Node::initTrfm() {
	Trfm3D I;
	m_placement->swap(I);
	// Update Geometric state
	updateGS();
}

void Node::setTrfm(const Trfm3D * M) {
	if (!M) {
		fprintf(stderr, "[E] setTrfm: no trfm for node %s\n", m_name.c_str());
		exit(1);
	}
	m_placement->clone(M);
	// Update Geometric state
	updateGS();
}

void Node::addTrfm(const Trfm3D * M) {
	if (!M) {
		fprintf(stderr, "[E] addTrfm: no trfm for node %s\n", m_name.c_str());
		exit(1);
	}
	m_placement->add(M);
	// Update Geometric state
	updateGS();
}

void Node::translate(const Vector3 & P) {
	static Trfm3D localT;
	localT.setTrans(P);
	addTrfm(&localT);
};

void Node::rotateX(float angle ) {
	static Trfm3D localT;
	localT.setRotX(angle);
	addTrfm(&localT);
};

void Node::rotateY(float angle ) {
	static Trfm3D localT;
	localT.setRotY(angle);
	addTrfm(&localT);
};

void Node::rotateZ(float angle ) {
	static Trfm3D localT;
	localT.setRotZ(angle);
	addTrfm(&localT);
};

void Node::scale(float factor ) {
	static Trfm3D localT;
	localT.setScale(factor);
	addTrfm(&localT);
};

///////////////////////////////////
// tree operations

Node *Node::parent() {
	if (!m_parent) return this;
	return m_parent;
}

/**
 * nextSibling: Get next sibling of node. Return first sibling if last child.
 *
 */

Node *Node::nextSibling() {
	Node *p = m_parent;
	if(!p) return this;
	list<Node *>::iterator end = p->m_children.end();
	list<Node *>::iterator it = p->m_children.begin();
	while(it != end && *it != this) ++it;
	assert(it != end);
	++it;
	if (it == end) return *(p->m_children.begin());
	return *it;
}

/**
 * firstChild: Get first child of node. Return itself if leaf node.
 *
 * @param theNode A pointer to the node
 */

Node *Node::firstChild() {
	if (!m_children.size()) return this;
	return *(m_children.begin());
}

// Cycle through children of a Node. Return children[ i % lenght(children) ]

Node * Node::cycleChild(size_t idx) {

	size_t m = idx % m_children.size();
	size_t i = 0;
	for(auto & theChild : m_children) {
		if (i == m) return theChild;
		i++;
	}
	return 0;
}

// @@ TODO:
//
// Add a child to node
// Print a warning (and do nothing) if node already has an gObject.

void Node::addChild(Node *theChild) {
	if (theChild == 0) return;
	if (m_gObject) {/*es un puntero a un objeto de clase objeto si 
	es 0 no hay si es cualquier otra cosa es true*/
	/* =================== PUT YOUR CODE HERE ====================== */
		//apunta a un objeto
		// node has a gObject, so print warning
		printf("Se trata de una hoja y no se le pueden añadir hijos");
		/* =================== END YOUR CODE HERE ====================== */
	} 
	else { //no es una hoja
		/* =================== PUT YOUR CODE HERE ====================== */
		/*Añadimos el hijo a la lista de hijos del padre(el padre es this)*/
		

		/*Añadir al padre en el hijo*/
		theChild->m_parent= this;
		theChild->m_placementWC->clone(this->m_placementWC);
		theChild->m_placementWC->add(theChild->m_placement);
		theChild->updateGS();
		this->m_children.push_back(theChild);//push_back para añadir
		/* =================== END YOUR CODE HERE ====================== */
	}
}

void Node::detach() {

	Node *theParent;
	theParent = m_parent;
	if (theParent == 0) return; // already detached (or root node)
	m_parent = 0;
	theParent->m_children.remove(this);
	// Update bounding box of parent
	theParent->propagateBBRoot();
}

// @@ TODO: auxiliary function
//
// given a node:
// - update the BBox of the node (updateBB)
// - Propagate BBox to parent until root is reached
//
// - Preconditions:
//    - the BBox of thisNode's children are up-to-date.
//    - placementWC of node and parents are up-to-date

void Node::propagateBBRoot() {
	/* =================== PUT YOUR CODE HERE ====================== */

	/* =================== END YOUR CODE HERE ====================== */
}

// @@ TODO: auxiliary function
//
// given a node, update its BBox to World Coordinates so that it includes:
//  - the BBox of the geometricObject it contains (if any)
//  - the BBox-es of the children
//
// Note: Bounding box is always in world coordinates
//
// Precontitions:
//
//     * m_placementWC is up-to-date
//     * m_containerWC of children are up-to-date
//
// These functions can be useful
//
// Member functions of BBox class (from Math/bbox.h):
//
//    void init(): Set BBox to be the void (empty) BBox
//    void clone(const BBox * source): Copy from source bounding box
//    void transform(const Trfm3D * T): Transform BBox by applying transformation T
//    void include(BBox *B): Change BBox so that it also includes BBox B
//
// Note:
//
//    See Recipe 1 at the beggining of the file in for knowing how to
//    iterate through children.

void Node::updateBB () {
	/* =================== PUT YOUR CODE HERE ====================== */

	/* =================== END YOUR CODE HERE ====================== */
}

// @@ TODO: Update WC (world coordinates matrix) of a node and
// its bounding box recursively updating all children.
//
// given a node:
//  - update the world transformation (m_placementWC) using the parent's WC transformation
//  - recursive call to update WC of the children
//  - update Bounding Box to world coordinates
//
// Precondition:
//
//  - m_placementWC of m_parent is up-to-date (or m_parent == 0)
//
// Note:
//
//    See Recipe 1 at the beggining of the file in for knowing how to
//    iterate through children.

void Node::updateWC() {
	 /*=================== PUT YOUR CODE HERE ====================== */
	
	/*if(this->m_parent){//si devuelve 0 significa que no tiene padre y por tanto es ROOT
		m_placementWC=this->m_placement;
	}
	else{
		m_placementWC->clone(m_parent->m_placementWC);//clonamos la matriz del padre
		m_placementWC->add(m_placement);//la multiplicamos
	}
		
	for (auto n: m_children){//llamada recursiva
		n->updateWC();//actualizamos la matriz WC de los hijos
	}*/
	



	
	if(this->m_parent==0){//si devuelve 0 significa que no tiene padre y por tanto es ROOT
		m_placementWC->clone(this->m_placement);
	}
	else{
		m_placementWC->clone(m_parent->m_placementWC);//clonamos la matriz del padre
		m_placementWC->add(m_placement);//la multiplicamos
		if(!m_gObject){//si no es nodo hoja
			for (auto n: m_children){//llamada recursiva
				n->updateWC();//actualizamos la matriz WC de los hijos
			}
		}
		

	}

	/*if(this->m_parent!=0){//si devuelve 0 significa que no tiene padre y por tanto es ROOT
		m_placementWC->clone(m_parent->m_placementWC);//clonamos la matriz del padre
		m_placementWC->add(m_placement);//la multiplicamos
	}
	
	for (auto n: m_children){//llamada recursiva
				n->updateWC();//actualizamos la matriz WC de los hijos
	}*/
		
		

	
	/* =================== END YOUR CODE HERE ====================== */
}

// @@ TODO:
//
//  Update geometric state of a node.
//
// given a node:
// - Update WC transformation of sub-tree starting at node (updateWC)
// - Propagate Bounding Box to root (propagateBBRoot), starting from the parent, if parent exists.

void Node::updateGS() {
	/* =================== PUT YOUR CODE HERE ====================== */
	this->propagateBBRoot();
	this->updateWC();
	
	/* =================== END YOUR CODE HERE ====================== */
}


// @@ TODO:
// Draw a (sub)tree.
//
// These functions can be useful:
//
// RenderState *rs = RenderState::instance();
//
// rs->addTrfm(RenderState::modelview, T); // Add T transformation to modelview
// rs->push(RenderState::modelview); // push current matrix into modelview stack
// rs->pop(RenderState::modelview); // pop matrix from modelview stack to current
//
// gobj->draw(); // draw geometry object (gobj)
//
// Note:
//
//    See Recipe 1 at the beggining of the file in for knowing how to
//    iterate through children.

void Node::draw() {

	ShaderProgram *prev_shader = 0;
	RenderState *rs = RenderState::instance();

	for(auto l : m_lights)
		l->placeScene(rs->top(RenderState::modelview), *m_placementWC);

	if (m_isCulled) return;

	// Set shader (save previous)
	if (m_shader != 0) {
		prev_shader = rs->getShader();
		rs->setShader(m_shader);
	}

	// Print BBoxes
	if((rs->getBBoxDraw() || m_drawBBox) &&
	   (this == Scene::instance()->get_display_node())) {
		BBoxGL::draw( m_containerWC);
	}
	/* =================== PUT YOUR CODE HERE ====================== */
	if(m_gObject){ //en caso de ser hoja-->dibujar
		rs->push(RenderState::modelview); // push current matrix into modelview stack
		rs->addTrfm(RenderState::modelview, m_placementWC);//
		m_gObject->draw();
		rs->pop(RenderState::modelview); // pop matrix from modelview stack to current
	}
	else{
		for (auto n: m_children){
			n->draw();
		}
	}

	
	/* =================== END YOUR CODE HERE ====================== */

	if (prev_shader != 0) {
		// restore shader
		rs->setShader(prev_shader);
	}
}

// Set culled state of a node's children

void Node::setCulled(bool culled) {
	m_isCulled = culled;
	for(auto & theChild : m_children) {
		theChild->setCulled(culled); // Recursive call
	}
}

// @@ TODO: Frustum culling. See if a subtree is culled by the camera, and
//          update m_isCulled accordingly.

void Node::frustumCull(Camera *cam) {
	/* =================== PUT YOUR CODE HERE ====================== */

	/* =================== END YOUR CODE HERE ====================== */
}

// @@ TODO: Check whether a BSphere (in world coordinates) intersects with a
// (sub)tree.
//
// Return a pointer to the Node which collides with the BSphere. 0
// if not collision.
//
// Note:
//
//    See Recipe 1 at the beggining of the file in for knowing how to
//    iterate through children.

const Node *Node::checkCollision(const BSphere *bsph) const {
	if (!m_checkCollision) return 0;
	/* =================== PUT YOUR CODE HERE ====================== */

	return 0; /* No collision */
   /* =================== END YOUR CODE HERE ====================== */
}

void Node::print_trfm(int sep) const {
	std::string delim(sep,' ');
	printf("%sNode:%s\n", delim.c_str(), m_name.c_str());
	m_placementWC->print(delim);
	for(auto & theChild : m_children) {
		theChild->print_trfm(sep + 1);
	}
}
