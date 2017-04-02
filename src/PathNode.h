#pragma once
#include <GLM\glm.hpp>
#include <map>
#include <string>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <memory>
#include "material.h"

struct PathNode 
{
	PathNode(int _distanceToTarget);
	PathNode(glm::vec3 _pos, int _distanceToTarget);
	PathNode(glm::vec3 _pos, int _distanceToTarget, bool _dummy);
	~PathNode();
	glm::vec3 pos;
	float height = 10.0f;

	std::map<std::string, PathNode *> connections;

	void addConnection(PathNode * connectionUp, PathNode * connectionRight, PathNode * connectionDown, PathNode * connectionLeft);
	
	void setMaterial(std::shared_ptr<Material> newMaterial) { nodeMaterial = newMaterial; }

	void checkConnections();
	void draw();


	PathNode * path = nullptr;

	bool dummy;

	int f = 0; // Score for node
	int g = 0; // Cost from start to this node
	int h = 0; // Estimated cost from node to target
	int distanceToTarget; //h

	GLfloat vertices[6];

	GLuint vao;
	GLuint vbo;
	std::shared_ptr<Material> nodeMaterial;
};