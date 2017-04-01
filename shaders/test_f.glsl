#version 400

uniform sampler2D u_tex;
uniform vec3 colour;
// Fragment Shader Inputs
//in VertexData
//{
	//vec3 normal;
in vec2 texCoord;
	//vec4 colour;
	//vec3 posEye;
//} vIn;

layout(location = 2) out vec4 FragColor;

void main()
{
	vec2 uv = texCoord.xy;
	FragColor = texture(u_tex, uv) + vec4(colour, 0.0);
}
