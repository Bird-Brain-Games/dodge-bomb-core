#version 400

uniform sampler2D u_bright;
uniform sampler2D u_scene;

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


void main()
{
	//////////////////////////////////////////////////////////////////////////
	// COMPOSITE BLOOM HERE
	////////////////////////////////////////////////////////////////////////// 
	vec4 composite = texture(u_bright, texCoord) * 0.5 + texture(u_scene, texCoord);
	FragColor = composite;
	FragColor.w = 1.0;
}