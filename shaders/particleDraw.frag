#version 330

uniform sample2D u_tex;

smooth in vec2 vTexCoord;
flat in vec4 vColorPart;

out vec4 FragColor;

void main()
{
 vec4 vTexColour = texture(u_tex, vTexCoord);
 fragColour = vec4(vTexColour.xyz, 1.0)*vColorPart;
}