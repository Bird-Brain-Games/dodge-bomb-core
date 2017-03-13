#pragma once

#include <memory>
#include <GL\glew.h>
#include <GLM\glm.hpp>

// holds the information necessary to display a texture 
// using blinn-phong shading
class Texture
{
public:
	Texture(char* _diffuseTex, char* _specularTex, float _shininess);

	// sends the values to the shader
	void bind(GLuint diffuseTexUnit, GLuint specularTexUnit);
	void unbind(GLuint diffuseTexUnit, GLuint specularTexUnit);

	void setShininess(float);

private:
	GLuint diffuseTex;
	GLuint specularTex;
	float shininess;
};