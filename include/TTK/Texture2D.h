//////////////////////////////////////////////////////////////////////////
//
// This header is a part of the Tutorial Tool Kit (TTK) library. 
// You may not use this header in your GDW games.
//
// This is a simple Texture2D class
//
// Michael Gharbharan 2015
//
//////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_2D_H
#define TEXTURE_2D_H

#include <string>
#include <GLEW/glew.h>

namespace TTK
{
	class Texture2D
	{
	public:
		Texture2D();
		Texture2D(std::string filename);
		~Texture2D();

		// Returns width / height of texture in pixels
		int width();
		int height();

		// Description:
		// Loads texture at specified location.
		// file path is relative to the executable.
		// If createGLTexture is true, the texture data will be sent to vram and a handle will be created
		// otherwise, only the data will be loaded (accessible with "data")
		void loadTexture(std::string fileName, bool createGLTexture = true, bool flip = false);

		// Description:
		// Calling this before drawing will make the mesh being drawn use
		// this texture, providing the mesh has texture coordinates.
		void bind(GLenum textureUnit = GL_TEXTURE0);

		void unbind(GLenum textureUnit = GL_TEXTURE0);

		// Returns texID
		unsigned int id();

		// Returns pointer to texture data
		unsigned char* data();

		int type();
		int format();

	private:
		unsigned int texWidth;
		unsigned int texHeight;
		GLenum texUnit;
		unsigned int texID;
		unsigned char* dataPtr;
		int dataType;
		int pixelFormat;
	};
}

#endif