#pragma once

#include <GLM/glm.hpp>
#include <string>

class PointHandle
{
public:
	// Point size is in Pixels
	PointHandle(float _pointSize, glm::vec3 _position, std::string _label = "");

	// Does a simple radius based intersection test
	bool isInside(glm::vec3 p);

	void draw();

	glm::vec3 position;
	std::string label;
	float pointSize;
};