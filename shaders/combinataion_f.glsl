#version 400

uniform sampler2D u_tex;
uniform sampler2D u_particle;
uniform sampler2D u_activater;

in vec2 texCoord;

layout(location = 0) out vec4 FragColor;

void main()
{
	vec4 colour = texture(u_tex, texCoord);
	vec4 particle = texture(u_particle, texCoord);
	vec4 activator = texture(u_activater, texCoord);
	
	FragColor = (colour * vec4(1 - activator.w)) + (particle * vec4(colour.w));
	
}