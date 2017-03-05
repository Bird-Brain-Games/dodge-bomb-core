#version 400

uniform sampler2D u_tex;

in vec2 texCoord;

layout(location = 0) out vec4 FragColor;

void main()
{
	vec2 uv = texCoord;
	FragColor = texture(u_tex, uv);
}