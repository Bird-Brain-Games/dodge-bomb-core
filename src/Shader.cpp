#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "GLEW/glew.h"

Shader::Shader()
{
	handle = 0;
}

Shader::~Shader()
{
	destroy();
}

unsigned int Shader::loadShaderFromFile(std::string fileName, GLenum type)
{
	// Load shader file into memory
	std::ifstream stream(fileName);
	std::stringstream buffer;
	buffer << stream.rdbuf();
	std::string shaderCode = buffer.str();

	// Could not load file
	if (shaderCode.length() == 0)
		return 0;

	// Create shader
	// Makes an empty shader program with nothing in it
	handle = glCreateShader(type);

	// Load shader code into shader program
	// [0] - which shader program to load code into
	// [1] - number of source files for the shader
	// [2] - pointer to an array of strings (pointer to pointer)
	// [3] - terminating character for source files
	const char* cstr = shaderCode.c_str();
	glShaderSource(handle, 1, &cstr, 0);

	// Compile the shader program
	glCompileShader(handle);

	// Check to see if shader compiled
	// Returns 1 if success
	int compileStatus;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus)
	{
		std::cout << "Shader Compiled Successfully: " << fileName << std::endl;
		return handle;
	}

	std::cout << "Shader Failed to Compile: " << fileName << std::endl;

	// If shader failed to compile, output the errors
	// First need to get length of error message
	int logLength;
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLength);

	// Make string for log
	// Note: type of quotes is important
	std::string log(logLength, ' ');

	// Get error log from OpenGL and store into string
	// Remember string is an array of characters internally
	glGetShaderInfoLog(handle, logLength, &logLength, &log[0]);

	// Output log to screen
	std::cout << log << std::endl;

	return 0;
}

void Shader::destroy()
{
	// If handle is zero it means the shader does not exist
	if (handle)
	{
		// Frees memory occupied by this shader
		glDeleteShader(handle);
		handle = 0;
	}
}

