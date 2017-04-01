#pragma once

#include "GLEW/glew.h"
#include "glm/glm.hpp"

#define MAX_BUFFERS 16

class FrameBufferObject
{
private:
	// Handle for FBO itself
	unsigned int handle;

	// Handle for texture attachments
	// Multiple colour textures can be attached to a single FBO
	// Fragment shader can output multiple values
	unsigned int colourTexHandles[MAX_BUFFERS];
	unsigned int numColourTex;

	// Handle for depth texture attachment
	// Can only have one depth texture
	// Depth is calculated based on vertex positions
	// Before the fragment shader
	unsigned int depthTexHandle;

	// Dimensions of textures
	unsigned int width, height;

	// Attachments
	GLenum bufferAttachments[MAX_BUFFERS];
	int numBuffers;

public:
	FrameBufferObject();
	~FrameBufferObject();

	void createFrameBuffer(unsigned int fboWidth, unsigned int fboHeight, unsigned int numBuffers, bool useDepth);

	// Set active framebuffer for rendering
	void bindFrameBufferForDrawing();
	static void unbindFrameBuffer(int backBufferWidth, int backBufferHeight);

	static void clearFrameBuffer(glm::vec4 clearColour);
	static void clearDepthBuffer(glm::vec4 clearColour);
	// Bind specific textures
	// Allows us to sample textures in a shader
	void bindTextureForSampling(int textureIndex, GLenum textureUnit);
	void unbindTexture(GLenum textureUnit);
	void bindDepthTextureForSampling(GLenum textureUnit);


	unsigned int getWidth() { return width; }
	unsigned int getHeight() { return height; }

	void destroy();
};