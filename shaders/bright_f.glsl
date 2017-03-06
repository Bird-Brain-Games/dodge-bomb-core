#version 400

uniform sampler2D u_tex;
uniform vec4 u_bloomThreshold;

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
	// IMPLEMENT BRIGHT PASS HERE
	////////////////////////////////////////////////////////////////////////// 
	// Original image color
	vec4 c = texture(u_tex, texCoord);

	FragColor = clamp((c - u_bloomThreshold.x) / (1.0 - u_bloomThreshold.x), 0.0, 1.0);
}