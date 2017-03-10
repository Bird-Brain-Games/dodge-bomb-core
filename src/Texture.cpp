#include "Texture.h"
#include "IL\ilut.h"


Texture::Texture(char* _diffuseTex, char* _specularTex, float _shininess)
{
	diffuseTex = ilutGLLoadImage(_diffuseTex);
	specularTex = ilutGLLoadImage(_specularTex);
	shininess = _shininess;
}

void Texture::bind(GLuint diffuseTexUnit, GLuint specularTexUnit)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind the diffuse
	glActiveTexture(diffuseTexUnit);
	glBindTexture(GL_TEXTURE_2D, diffuseTex);

	// Bind the specular
	glActiveTexture(specularTexUnit);
	glBindTexture(GL_TEXTURE_2D, specularTex);
}

void Texture::unbind(GLuint diffuseTexUnit, GLuint specularTexUnit)
{
	glActiveTexture(diffuseTexUnit);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(specularTexUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

void Texture::setShininess(float shine)
{
	shininess = shine;
}