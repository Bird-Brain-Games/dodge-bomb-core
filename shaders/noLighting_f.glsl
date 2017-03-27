#version 400

uniform vec4 u_colour;

uniform sampler2D u_diffuseTex;
uniform sampler2D u_specularTex;

// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec2 texCoord;
	vec4 colour;
	vec3 posEye;
	vec3 posInLight;
} vIn;

layout(location = 0) out vec4 FragColor;

void main()
{
	vec4 diffuseColour = texture2D(u_diffuseTex, vIn.texCoord.st);

	FragColor = vec4(diffuseColour);
}