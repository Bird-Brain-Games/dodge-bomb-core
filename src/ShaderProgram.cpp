#include "ShaderProgram.h"
#include <iostream>

ShaderProgram::ShaderProgram(std::string _name)
{
	name = _name;
	handle = 0;
}

ShaderProgram::~ShaderProgram()
{
	destroy();
}

void ShaderProgram::attachShader(Shader shader)
{
	if (handle == 0)
	{
		handle = glCreateProgram();
	}

	if (shader.getHandle())
	{
		glAttachShader(handle, shader.getHandle());
	}
}

int ShaderProgram::linkProgram()
{
	if (handle)
	{
		// Link the shaders together into a single program
		glLinkProgram(handle);

		// Check to see if shader program linked
		// Returns 1 if success
		int linkStatus;
		glGetProgramiv(handle, GL_LINK_STATUS, &linkStatus);

		if (linkStatus)
		{
			std::cout << "Shader " + name + " linked Successfully." << std::endl;
			return handle;
		}

		// If shader failed to link, output the errors
		// First need to get length of error message
		int logLength;
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &logLength);

		// Make string for log
		// Note: type of quotes is important
		std::string log(logLength, ' ');

		// Get error log from OpenGL and store into string
		// Remember string is an array of characters internally
		glGetProgramInfoLog(handle, logLength, &logLength, &log[0]);

		// Output log to screen
		std::cout << log << std::endl;
	}
	else
	{
		std::cout << "Shader "+ name +" program failed to link: handle not set" << std::endl;
	}
}

void ShaderProgram::bind()
{
	glUseProgram(handle);
}

void ShaderProgram::unbind()
{
	glUseProgram(0);
}

void ShaderProgram::sendUniformInt(const std::string& uniformName, int intVal)
{
	int uniformLocation = getUniformLocation(uniformName);
	glUniform1i(uniformLocation, intVal);
}

void ShaderProgram::sendUniformVec4(const std::string& uniformName, glm::vec4& vec4)
{
	int uniformLocation = getUniformLocation(uniformName);
	glUniform4fv(uniformLocation, 1, &vec4[0]);
}

void ShaderProgram::sendUniformVec2(const std::string& uniformName, glm::vec2& vec2)
{
	int uniformLocation = getUniformLocation(uniformName);
	glUniform4fv(uniformLocation, 1, &vec2[0]);
}

void ShaderProgram::sendUniformMat4(const std::string& uniformName, glm::mat4& mat4, int _num)
{
	int uniformLocation = getUniformLocation(uniformName);
	glUniformMatrix4fv(uniformLocation, _num, false, &mat4[0][0]);
}

void ShaderProgram::destroy()
{
	if (handle)
	{
		glDeleteProgram(handle);
	}
}

int ShaderProgram::getUniformLocation(const std::string& uniformName)
{
	//create a copy of an element in the map.
	std::unordered_map<std::string, int>::iterator i = location.find(uniformName);

	//checks whether it the element actually existed
	if (i == location.end())
	{
		//if not found add the uniform to the list
		location[uniformName] = glGetUniformLocation(handle, uniformName.c_str());
		//check whether the value is actually being sent to our shader
		i = location.find(uniformName);
		if (i->second == -1)
		{
			std::cout << "error in shader " << name << " " << uniformName << " does not exist" << std::endl;
		}
	}
	return i->second;

}
