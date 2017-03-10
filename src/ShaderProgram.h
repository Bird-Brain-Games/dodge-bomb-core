#pragma once

#include "Shader.h"
#include <glm\matrix.hpp>
#include "GLEW/glew.h"
#include <unordered_map>

class ShaderProgram
{
public:
	ShaderProgram(std::string);
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
	void sendUniformMat4(const std::string& uniformName, glm::mat4& mat4, int _num = 1);

	void destroy();

	unsigned int getHandle() { return handle; }

private:
	//name used for outputing error messages.
	std::string name;
	unsigned int handle;


	//we use unordered map because its faster to look up info. complexity of o(1) vs o(logn) for a normal map
	std::unordered_map<std::string, int> location;
	// All uniforms have a constant location
	// This function searches for the uniform name (as written in the shader)
	// and stores it in the above map and then returns it.
	// we store it because the call to get the location is a weight call.
	int getUniformLocation(const std::string& uniformName);
};