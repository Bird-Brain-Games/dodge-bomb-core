#pragma once
#include "GLM\glm.hpp"
#include <vector>
#include "VAO.h"
#include "Shader.h"
class Loader
{
public:
	virtual bool load(char const* filename) { return false; };
	virtual void draw(std::shared_ptr<Shader> s) = 0;
	virtual void createVAO() = 0;
	virtual void update(float dt) {};

protected:

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords; //note: vec2

	VAO vao;
};