// loadObject.h
// - Wavefront OBJ loader
// - Only takes in faces with 3 points (triangles)
// - Only the most basic functionality

#pragma once
#include <memory>
#include <GL\glew.h>
#include <GLM\glm.hpp>
#include <vector>
#include "VAO.h"
class Shader;

// we're loading a Wavefront OBJ
class LoadObject
{
public:
	bool loadFromObject(char const* filename);
	void draw();
private:
	void createVAO();

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords; //note: vec2

	VAO vao;
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