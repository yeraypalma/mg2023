#include <iterator>
#include <cstdio>
#include <string>
#include "tools.h"
#include "gObject.h"
#include "triangleMeshGL.h"

using std::list;
using std::string;

GObject::GObject(const std::string & name) : m_name(name) {}
GObject::~GObject() {
	for(auto & mesh : m_meshes)
		delete mesh;

	for(auto & mesh : m_meshes_transp)
		delete mesh;
}

GObject *GObject::createFromWavefront(const string & DirName, const string & FileName ) {

	GObject *newGObject;
	TriangleMesh *oneMesh;
	list<TriangleMesh *> auxlist;

	newGObject = new GObject(getFilename(DirName, FileName));
	TriangleMesh::CreateTMeshObj(DirName, FileName, auxlist);
	for(auto & mesh : auxlist)
		newGObject->add(mesh);
	return newGObject;
}

void GObject::add(TriangleMesh *oneMesh) {

	if(oneMesh->getMaterial()->isTransp()) {
		m_meshes_transp.push_back(oneMesh);
	} else {
		m_meshes.push_back(oneMesh);
	}
	updateContainer();
}

const char *GObject::getName() const { return m_name.c_str(); }
const std::string & GObject::getNameString() const { return m_name; }

void GObject::draw() {
	drawOpaque();
	drawTransparent();
}

void GObject::drawOpaque() {
	for(auto & mesh : m_meshes)
		TriangleMeshGL::draw(mesh);
}

void GObject::drawTransparent() {
	for(auto & mesh : m_meshes_transp)
		TriangleMeshGL::draw(mesh);
}

const BBox * GObject::getContainer() {
	return &m_container;
}

void GObject::updateContainer() {

	TriangleMesh  *oneMesh;
	float    xmin, xmax, ymin, ymax, zmin, zmax;

	for(auto & mesh : m_meshes)
		mesh->includeBBox(m_container);

	for(auto & mesh : m_meshes_transp)
		mesh->includeBBox(m_container);
}

// apply a trfm to all meshes
void GObject::applyTrfm(const Trfm3D * trfm) {
	for(auto & mesh : m_meshes)
		mesh->applyTrfm(trfm);

	for(auto & mesh : m_meshes_transp)
		mesh->applyTrfm(trfm);
	updateContainer();
}

// Set gObject to be faceted.
void GObject::setFaceted() {
	for(auto & mesh : m_meshes)
		mesh->setFaceted();

	for(auto & mesh : m_meshes_transp)
		mesh->setFaceted();
}

// Set gObject to be smooth.
void GObject::setSmooth() {
	for(auto & mesh : m_meshes)
		mesh->setSmooth();

	for(auto & mesh : m_meshes_transp)
		mesh->setSmooth();
}


int  GObject::numTriangles() const {
	int      numPols = 0;
	for(auto & mesh : m_meshes)
		numPols += mesh->numTriangles();

	for(auto & mesh : m_meshes_transp)
		numPols += mesh->numTriangles();

	return numPols;
}

size_t GObject::size() const {
	return m_meshes.size() + m_meshes_transp.size();
}

TriangleMesh *GObject::at(size_t idx) {
	const std::list<TriangleMesh *> *ptr = &m_meshes;
	if (idx > m_meshes.size()) {
		ptr = &m_meshes_transp;
		idx -= m_meshes.size();
	}
	std::list<TriangleMesh *>::const_iterator it = ptr->begin();
	std::advance(it, idx);
	return *it;
}

void GObject::setMaterial(Material *mat) {
	for(auto & mesh : m_meshes)
		mesh->setMaterial(mat);

	for(auto & mesh :m_meshes_transp)
		mesh->setMaterial(mat);
}

	// Set texture to all meshes inside this gObject
void  GObject::setTexture(Texture *tex) {
	Material *mat;
	for(auto & mesh : m_meshes) {
		mat = mesh->getMaterial();
		mat->setTexture(tex);
	}
	for(auto & mesh : m_meshes_transp) {
		mat = mesh->getMaterial();
		mat->setTexture(tex);
	}
}


void GObject::print() const {
	printf("*** Opaque meshes:\n");
	for(auto & mesh : m_meshes)
		mesh->print();

	printf("*** Transparent meshes:\n");
	for(auto & mesh : m_meshes_transp)
		mesh->print();

}
