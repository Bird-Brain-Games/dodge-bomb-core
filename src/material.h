/*Campbell Hamilton 100582048
january 2017
*/

#pragma once
#include "Shader.h"
#include <memory>
#include <map>
#include <glm\glm.hpp>
class Material
{
public:
	//our materials shader
	std::shared_ptr<Shader> shader;
	// the unifrom data
	std::map<std::string, glm::vec4> vec4Uniforms;
	std::map<std::string, glm::mat4> mat4Uniforms;
	std::map<std::string, int> intUniforms;

	Material();
	//sends the uniform data to our shader
	void sendUniforms();

};
