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

	void checkConnections();

	bool dummy;

	int distanceToTarget; //h
};