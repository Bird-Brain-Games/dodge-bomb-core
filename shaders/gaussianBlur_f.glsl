#version 400

uniform sampler2D u_tex;

// (1.0 / windowWidth, 1.0 / windowHeight)
uniform vec4 u_texelSize; // Remember to set this!
uniform vec4 u_kernel;


// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 posEye;
} vIn;

in vec2 texCoord;

layout(location = 0) out vec4 FragColor;

vec3 gaussianBlur(sampler2D tex)
{
	vec2 uv = (texCoord);

	vec2 offsetCoordinates[9];

	// left column
	offsetCoordinates[0] = vec2(-u_texelSize.x, u_texelSize.y) + uv;	// top left
	offsetCoordinates[1] = vec2(-u_texelSize.x, 0.0) + uv;				// mid left
	offsetCoordinates[2] = vec2(-u_texelSize.x, -u_texelSize.y) + uv;	// bot left

																		// mid column
	offsetCoordinates[3] = vec2(0.0, u_texelSize.y) + uv;				// top mid
	offsetCoordinates[4] = vec2(0.0, 0.0) + uv;							// mid mid
	offsetCoordinates[5] = vec2(0.0, -u_texelSize.y) + uv;				// bot mid

																		// right column
	offsetCoordinates[6] = vec2(u_texelSize.x, u_texelSize.y) + uv;		// top right
	offsetCoordinates[7] = vec2(u_texelSize.x, 0.0) + uv;				// mid right
	offsetCoordinates[8] = vec2(u_texelSize.x, -u_texelSize.y) + uv;	// bot right

	vec3 blurred = vec3(0.0);

	// if doing gaussian blur, multiply each sample by weight
	blurred += texture(tex, offsetCoordinates[0]).rgb * u_kernel.x;
	blurred += texture(tex, offsetCoordinates[1]).rgb * u_kernel.y;
	blurred += texture(tex, offsetCoordinates[2]).rgb * u_kernel.x;

	blurred += texture(tex, offsetCoordinates[3]).rgb * u_kernel.y;
	blurred += texture(tex, offsetCoordinates[4]).rgb * u_kernel.z;
	blurred += texture(tex, offsetCoordinates[5]).rgb * u_kernel.y;

	blurred += texture(tex, offsetCoordinates[6]).rgb * u_kernel.x;
	blurred += texture(tex, offsetCoordinates[7]).rgb * u_kernel.y;
	blurred += texture(tex, offsetCoordinates[8]).rgb * u_kernel.x;

	return blurred;
}

void main()
{
	//////////////////////////////////////////////////////////////////////////
	// IMPLEMENT 2D GAUSSIAN BLUR HERE
	////////////////////////////////////////////////////////////////////////// 
	vec3 gaussian = gaussianBlur(u_tex);
	FragColor = vec4(gaussian, 1.0);
}