#version 330

in vec2 UV;
in vec3 normal0;
out vec4 outputColor;

uniform sampler2D diffuseTex;

void main()
{
	outputColor = texture2D(diffuseTex, UV);
}