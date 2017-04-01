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

in vec2 texCoord;

layout(location = 0) out vec4 FragColor;

void main()
{
	///	TODO: Composite Bokehs here
	vec4 A = texture(u_bokehA, texCoord);
	vec4 B = texture(u_bokehB, texCoord);

	FragColor = min(A, B);
}