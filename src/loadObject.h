// loadObject.h
// - Wavefront OBJ loader
// - Only takes in faces with 3 points (triangles)
// - Only the most basic functionality

#pragma once
#include <memory>
#include <GL\glew.h>
#include <GLM\glm.hpp>
#include "loader.h"
class Shader;

// we're loading a Wavefront OBJ
class LoadObject: public Loader
{
public:
	bool load(char const* filename);
	void draw(std::shared_ptr<Shader> s);
	
private:
	void createVAO();
};

// holds the information necessary to display a texture 
// using blinn-phong shading
class Texture
{
public:
	Texture(char* _diffuseTex, char* _specularTex, float _shininess);
	Texture(GLuint _diffuseTex, GLuint _specularTex, float _shininess);

	// sends the values to the shader
	void bind(std::shared_ptr<Shader> s);
	void setShininess(float);

private:
	GLuint diffuseTex;
	GLuint specularTex;
	float shininess;
};