#version 400

uniform sampler2D u_scene;
uniform sampler2D u_bright;
uniform sampler2D u_dirt;

// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec2 texCoord;
	vec4 colour;
	vec3 posEye;
} vIn;

layout(location = 0) out vec4 FragColor;
in vec2 texCoord;

void main()
{
	//////////////////////////////////////////////////////////////////////////
	// COMPOSITE BLOOM HERE
	////////////////////////////////////////////////////////////////////////// 
	vec4 bright = texture(u_bright, texCoord);
	vec4 dirt = texture(u_dirt, texCoord);
	vec4 scene = texture(u_scene, texCoord);

	vec4 composite =  (bright * dirt ) + (scene);
	FragColor = composite;
	FragColor.a = 1.0;
}