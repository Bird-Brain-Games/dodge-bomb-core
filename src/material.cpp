/*Campbell Hamilton 100582048
january 2017
*/

#include "material.h"


Material::Material() : shader(std::make_shared<Shader>())
{}

void Material::sendUniforms()
{
	// Send vector4 uniforms
	for (auto itr = vec4Uniforms.begin(); itr != vec4Uniforms.end(); itr++)
		shader->uniformVec3(itr->first, itr->second);

	// Send mat4 uniforms
	for (auto itr : mat4Uniforms)
		shader->uniformMat4x4(itr.first, &itr.second);
	//fix
};
