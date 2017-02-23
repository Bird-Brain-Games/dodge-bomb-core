// loadObject.h
// - Wavefront OBJ loader
// - Only takes in faces with 3 points (triangles)
// - Only the most basic functionality

#pragma once
#include <memory>
#include <string>
#include <GL\glew.h>
#include <GLM\glm.hpp>
#include "loader.h"
class Shader;

// we're loading a Wavefront OBJ
class LoadObject: public Loader
{
public:
	bool load(char const* filename);
	bool load(std::string filename);
	void draw(std::shared_ptr<ShaderProgram> s);
	
private:
	void createVAO();
};