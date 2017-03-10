#pragma once

#include "ShaderProgram.h"
#include <map>

class Material
{
public:
	std::shared_ptr<ShaderProgram> shader;

	std::map<std::string, glm::vec4> vec4Uniforms;
	std::map<std::string, glm::mat4> mat4Uniforms;
	std::map<std::string, int> intUniforms;
	// maps for other uniform types ...

	//the name of our material
	std::string name;

	Material(std::string _name)
		: shader(std::make_shared<ShaderProgram>(_name))
	{name = _name;}

	void sendUniforms()
	{
		// Send vector4 uniforms
		for (auto itr = vec4Uniforms.begin(); itr != vec4Uniforms.end(); itr++)
			shader->sendUniformVec4(itr->first, itr->second);

		// Send mat4 uniforms
		for (auto itr = mat4Uniforms.begin(); itr != mat4Uniforms.end(); itr++)
			shader->sendUniformMat4(itr->first, itr->second);

		// Send int uniforms
		for (auto itr = intUniforms.begin(); itr != intUniforms.end(); itr++)
			shader->sendUniformInt(itr->first, itr->second);
	}
};
