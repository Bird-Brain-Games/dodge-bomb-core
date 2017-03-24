#include "Texture.h"
#include "IL\ilut.h"


Texture::Texture(std::string _diffuseTex, std::string _specularTex, float _shininess)
{
	diffuseTex = std::make_shared<TTK::Texture2D>();
	specularTex = std::make_shared<TTK::Texture2D>();

	if (_diffuseTex == _specularTex)
	{
		diffuseTex->loadTexture(_diffuseTex);
		specularTex = diffuseTex;
	}
	else
	{
		diffuseTex->loadTexture(_diffuseTex);
		specularTex->loadTexture(_specularTex);
	}

	shininess = _shininess;
}

void Texture::bind(GLuint diffuseTexUnit, GLuint specularTexUnit)
{
	diffuseTex->bind(diffuseTexUnit);
	specularTex->bind(specularTexUnit);
}

void Texture::unbind(GLuint diffuseTexUnit, GLuint specularTexUnit)
{
	diffuseTex->unbind(diffuseTexUnit);
	specularTex->unbind(specularTexUnit);
}

void Texture::setShininess(float shine)
{
	shininess = shine;
}