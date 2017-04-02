#pragma once

#include <memory>
#include <GL\glew.h>
#include <GLM\glm.hpp>
#include "Texture2D.h"

// holds the information necessary to display a texture 
// using blinn-phong shading
class Texture
{
public:
	Texture(std::string _diffuseTex , std::string _specularTex, float _shininess);

	// sends the values to the shader
	void bind(GLuint diffuseTexUnit, GLuint specularTexUnit);
	void unbind(GLuint diffuseTexUnit, GLuint specularTexUnit);

	void setShininess(float);

private:
	std::shared_ptr<TTK::Texture2D> diffuseTex;
	std::shared_ptr<TTK::Texture2D> specularTex;
	float shininess;
};