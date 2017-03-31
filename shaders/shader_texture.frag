#version 330

in vec2 UV;
out vec4 outputColor;

uniform sampler2D diffuseTex;

void main()
{
	outputColor = texture2D(diffuseTex, UV);
}