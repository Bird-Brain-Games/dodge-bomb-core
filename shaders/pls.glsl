#version 400

uniform sampler2D u_scene;
uniform sampler2D u_bright;

// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec2 texCoord;
	vec4 colour;
	vec3 posEye;
} vIn;

layout(location = 0) out vec4 FragColor;


void main()
{
	//////////////////////////////////////////////////////////////////////////
	// COMPOSITE BLOOM HERE
	////////////////////////////////////////////////////////////////////////// 
	vec4 composite = texture(u_bright, vIn.texCoord);
	FragColor = composite;
	FragColor.a = 1.0;
}