// -*-C++-*-
#pragma once

#include "node.h"

class Scene {

public:
	static Scene * instance();

	Node *rootNode();
	void attach(Node *theChild);
	void draw();

	Node *get_display_node() const;
	void set_display_node(Node *dn);

	/**
	 * Set shading type to the scene
	 *
	 * @param face (GL_FRONT GL_BACK or GL_FRONT_AND_BACK).
	 * @param mode (GL_POINT, GL_LINE,GL_FILL). default GL_FILL.
	 */

	void setShading(GLenum face, GLenum mode);

	void print_scene_trfm() const;

private:
	Scene();
	~Scene();
	Scene(const Scene &);
	Scene & operator =(const Scene &);

	Node *m_rootNode; // root node
	Node *m_displayNode; // the selected node
};
