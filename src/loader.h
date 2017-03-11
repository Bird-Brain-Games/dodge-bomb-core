#pragma once
#include "GLM\glm.hpp"
#include <vector>
#include "VAO.h"
#include "ShaderProgram.h"
class Loader
{
public:
	virtual bool load(char const* filename) { return false; };
	virtual void draw(std::shared_ptr<ShaderProgram> s) = 0;
	virtual void createVAO() = 0;
	virtual std::vector<glm::mat4> update(float dt, float overRide, float counter) { std::vector<glm::mat4> IHE;  return IHE; };
	virtual void setAnim(std::string) {};
protected:

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords; //note: vec2

	VAO vao;
};