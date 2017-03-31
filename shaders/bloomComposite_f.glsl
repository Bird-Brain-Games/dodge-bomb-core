#version 400

uniform sampler2D u_bright;
uniform sampler2D u_scene;

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
	vec4 composite = texture(u_bright, vIn.texCoord) * 0.5 + texture(u_scene, vIn.texCoord) * 0.5;
	FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}