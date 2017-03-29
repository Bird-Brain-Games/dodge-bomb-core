#version 400

uniform vec4 u_lightPos;
uniform vec4 u_colour;

// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec2 texCoord;
	vec4 colour;
	vec3 posEye;
	vec3 posInLight; // light space (shadow map)
} vIn;


layout(location = 0) out vec4 FragDepth;

void main()
{
	float depthEye = vIn.posEye.z;
	// The RGB channels contain the pixel's color
	FragDepth.rgb = vec3(abs(depthEye * 0.01));
	FragDepth.a = 1.0;
}