#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLM\glm.hpp>

class Shader
{
public:
	Shader(const char *, const char *, const char * = NULL);
	Shader();
	~Shader(void);
	void load(const char *, const char *, const char * = NULL);

	void init(const char *, const char *, const char * = NULL);
	
	void bind();		// use the shader vert and frag
	void unbind();		// stop using shader vert and frag

	unsigned int getID();

	void uniformVec3(const std::string&, float*);
	void uniformVec3(const std::string&, float, float, float);
	void uniformVec3(const std::string&, glm::vec3*);

	void uniformVec3(const std::string&, glm::vec4&);
	void uniformVec4(const std::string& varName, glm::vec4& v);

	void uniformFloat(const std::string&, float );
	void uniformTex(const std::string&, GLuint , unsigned short);
	void uniformMat4x4(const std::string&, glm::mat4x4*, unsigned int size = 1);
	void uniformInt(const std::string& varName, int data);
	void uniformVec2(const std::string& varName, glm::vec2& v);

	int getUnifrom(const std::string&);
	//------------------------------------------------------------------------
	// Variables

	//glsl program handler
	unsigned int programID;

	//vert and fag shaders
	unsigned int vertShader;
	unsigned int fragShader;
	unsigned int geomShader;


};

