#include "FrameBufferObject.h"
#include <iostream>
 
FrameBufferObject::FrameBufferObject()
{
	depthTexHandle = 0;
	numColourTex = 0;
	memset(colourTexHandles, 0, MAX_BUFFERS);
	memset(bufferAttachments, 0, MAX_BUFFERS);
	numBuffers = 0;
}

FrameBufferObject::~FrameBufferObject()
{
	destroy();
}

// Render buffers do not have colour attachments and are used to store data
// for OpenGL logic operations such as depth testing.
// Frame Buffers do not store any actual data but instead we attach textures to them.
// We can write to the textures in a fragment shader.

void FrameBufferObject::createFrameBuffer(unsigned int fboWidth, unsigned int fboHeight, unsigned int numColourBuffers, bool useDepth)
{
	width = fboWidth;
	height = fboHeight;
	numColourTex = numColourBuffers;

	if (numColourTex > MAX_BUFFERS)
	{
		std::cout << "Attempting to create " << numColourTex << " buffers. Max is " << MAX_BUFFERS << std::endl;
		numColourTex = MAX_BUFFERS;
	}

	// Creating a handle for a FBO
	// This is an empty FBO
	glGenFramebuffers(1, &handle);

	// Bind the FBO
	// Tell OpenGL we want to do things to this FBO
	glBindFramebuffer(GL_FRAMEBUFFER, handle);

	// An FBO can be thought of as a collection of textures
	// But these aren't textures loaded in from file, these are
	// textures we render to at run time.
	// The textures attached to an FBO are referred to as 'attachments'
	// There are colour attachments (RGB) and a depth attachment (D)
	// This is where we generate these empty textures
	glGenTextures(numColourTex, colourTexHandles);

	// Since we just created a bunch of textures, we need to initialize them
	// This is similar to how you would initialize a texture loaded from file
	for (int i = 0; i < numColourTex; i++) // for each texture...
	{
		// ... bind it
		// binding tells OpenGL we want to do something with this texture
		glBindTexture(GL_TEXTURE_2D, colourTexHandles[i]);

		// We need to initialize the size of the texture
		// Here I am making each texture the same size, but you may want to
		// extend this class to allow textures of different size
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		// Texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Now that we have created handles and allocated memory for the texture
		// We need to add them to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colourTexHandles[i], 0);

		// Need to keep track of the attachments added 
		bufferAttachments[i] = GL_COLOR_ATTACHMENT0 + i;
		numBuffers++;
	}
	
	// An FBO can have a single depth map
	// Think about it, depth is calculated based on vertex positions, only one set of vertices get pass through pipeline at a time
	if (useDepth)
	{
		glGenTextures(1, &depthTexHandle);
		glBindTexture(GL_TEXTURE_2D, depthTexHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexHandle, 0);
	}

	// Set up which layout location in the fragment shader goes to which
	// colour attachment. this is a part of the fbo's state so we do not
	// have to call it every frame
	glDrawBuffers(numBuffers, bufferAttachments);

	// We just did a lot of stuff
	// It's a good idea to check if we did everything correctly
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Error Creating FBO " << fboStatus << std::endl;
	}

	// Unbind FBO
	// When we unbind an FBO it goes back to the system provided FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObject::bindFrameBufferForDrawing()
{
	glBindFramebuffer(GL_FRAMEBUFFER, handle);
	glViewport(0, 0, width, height);
}

void FrameBufferObject::unbindFrameBuffer(int backBufferWidth, int backBufferHeight)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, backBufferWidth, backBufferHeight);
}

void FrameBufferObject::clearFrameBuffer(glm::vec4 clearColour)
{
	glClearColor(clearColour.x, clearColour.y, clearColour.z, clearColour.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void FrameBufferObject::bindTextureForSampling(int textureIndex, GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, colourTexHandles[textureIndex]);
}

void FrameBufferObject::unbindTexture(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBufferObject::destroy()
{
	if (numColourTex > 0)
	{
		glDeleteTextures(numColourTex, colourTexHandles);
		memset(colourTexHandles, 0, MAX_BUFFERS);
		numColourTex = 0;
	}

	if (depthTexHandle)
	{
		glDeleteTextures(1, &depthTexHandle);
		depthTexHandle = 0;
	}

	if (handle)
		glDeleteFramebuffers(1, &handle);

	unbindFrameBuffer(width, height);
}
