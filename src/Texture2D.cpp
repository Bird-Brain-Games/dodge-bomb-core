#include "GLEW/glew.h"
#include "Texture2D.h"
#include "IL/ilut.h"

TTK::Texture2D::Texture2D()
{
	texWidth = texHeight = 0;
}


TTK::Texture2D::Texture2D(std::string filename)
{
	texWidth = texHeight = 0;
	loadTexture(filename);
}

TTK::Texture2D::~Texture2D()
{
	glDeleteTextures(1, &texID);
}

int TTK::Texture2D::width()
{
	return texWidth;
}

int TTK::Texture2D::height()
{
	return texHeight;
}

unsigned int TTK::Texture2D::loadTexture(std::string filename, bool createGLTexture, bool flip)
{
	glEnable(GL_TEXTURE_2D);

	ilGenImages(1, &texID);
	ilBindImage(texID);

	// DevIL's flip functionality seems to be broken for textures larger than 1024 x 1024...

	//ilEnable(IL_ORIGIN_SET);
	//if (flip)
	//	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	//else
	//	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

	ILboolean ret = ilLoadImage(filename.c_str()); // location relative to executable
	texWidth = ilGetInteger(IL_IMAGE_WIDTH);
	texHeight = ilGetInteger(IL_IMAGE_HEIGHT);
	dataType = ilGetInteger(IL_IMAGE_TYPE);
	pixelFormat = ilGetInteger(IL_IMAGE_FORMAT);

	dataPtr = ilGetData();

	// ... so lets flip the texture manually!
	if (flip)
	{
		int numChannels = 4;

		if (pixelFormat == IL_RGBA)
			numChannels = 4;
		else if (pixelFormat = IL_RGB)
			numChannels = 3;

		unsigned char* flippedTex = new unsigned char[texWidth * texHeight * numChannels];

		for (int y = 0; y < texHeight; y++)
		{
			for (int x = 0; x < texWidth; x++)
			{
				int rIdx = ((y * texWidth) + x) * numChannels;
				int gIdx = rIdx + 1;
				int bIdx = rIdx + 2;
				int aIdx = rIdx + 3;

				int ilIdx = ((((texWidth - 1) - y) * texWidth) + x) * numChannels;

				flippedTex[rIdx] = dataPtr[ilIdx];
				flippedTex[gIdx] = dataPtr[ilIdx + 1];
				flippedTex[bIdx] = dataPtr[ilIdx + 2];

				if (numChannels == 4)
					flippedTex[aIdx] = dataPtr[ilIdx + 3];
			}
		}
		dataPtr = flippedTex;
	}

	if (createGLTexture)
	{
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, texWidth, texHeight, 0, pixelFormat, dataType, dataPtr);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	ILenum Error;
	while ((Error = ilGetError()) != IL_NO_ERROR)
	{
		printf("Texture Loading Error:\t%d:\t%s\t %s\n", Error, filename.c_str(), iluErrorString(Error));
	}

	if (flip)
		delete[] dataPtr;

	return texID;
}

void TTK::Texture2D::bind(GLenum textureUnit /* = GL_TEXTURE0 */)
{
	/*	glEnable(GL_TEXTURE_2D);*/

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, texID);
}

void TTK::Texture2D::unbind(GLenum textureUnit /* = GL_TEXTURE0 */)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

unsigned int TTK::Texture2D::id()
{
	return texID;
}

unsigned char* TTK::Texture2D::data()
{
	return dataPtr;
}

int TTK::Texture2D::type()
{
	return dataType;
}

int TTK::Texture2D::format()
{
	return pixelFormat;
}