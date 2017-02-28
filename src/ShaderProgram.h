#pragma once

#include "Shader.h"
#include <glm\matrix.hpp>
#include "GLEW/glew.h"

class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	// Initialization functions
	void attachShader(Shader shader);
	int linkProgram();
	
	// Usage functions
	void bind();
	void unbind();

	// Functions to send uniforms to GPU from CPU

	// Sends a single integer value to GPU
	// Useful for assigning textures to samplers 
	void sendUniformInt(const std::string& uniformName, int intVal);

	// Sends four floats stored in an array to GPU
	// Useful for sending vector4 to GPU
	void sendUniformVec4(const std::string& uniformName, glm::vec4& vec4);

	// Sends a 4x4 matrix stored in an array to GPU
	// Must have to apply the MVP transform
	void sendUniformMat4(const std::string& uniformName, glm::mat4& mat4);

	void destroy();

	unsigned int getHandle() { return handle; }

private:
	unsigned int handle;

	// All uniforms have a constant location
	// This function searches for the uniform name (as written in the shader)
	// and returns that location. If name is not found, it returns 0
	// Note: This is a pretty slow operation and you do not want to be
	// calling this every frame. Set up a system, such as a std::map which caches
	// the location and used the string name as a key
	int getUniformLocation(const std::string& uniformName);
};