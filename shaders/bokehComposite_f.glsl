//Robert Savaglio, 100591436
#version 400

uniform sampler2D u_bokehA;
uniform sampler2D u_bokehB;

// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 posEye;
} vIn;

layout(location = 0) out vec4 FragColor;

void main()
{
	///	TODO: Composite Bokehs here
	vec4 A = texture(u_bokehA, vIn.texCoord.st);
	vec4 B = texture(u_bokehB, vIn.texCoord.st);

	FragColor = min(A, B);
}