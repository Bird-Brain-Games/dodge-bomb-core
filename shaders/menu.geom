#version 330

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

//dimensions 

in DETAILS
{
vec2 dim;
vec2 spot;
} details[1];

out vec2 UV;

void main()
{
	//The size of each texture on our atlas
	vec2 size = vec2(1.0) / details[0].dim;
	
	vec2 length = vec2(size.x * details[0].spot.x, size.x * (details[0].spot.x+1));
	vec2 height = vec2(size.y * details[0].spot.y, size.y * (details[0].spot.y+1));
	
	gl_Position = vec4(1.0, 1.0, 0.0, 1.0);
	UV = vec2(length.y, height.y);
	EmitVertex();
	
	gl_Position = vec4(-1.0, 1.0, 0.0, 1.0);
	UV = vec2(length.x, height.y);
	EmitVertex();
	
	gl_Position = vec4(1.0, -1.0, 0.0, 1.0);
	UV = vec2(length.y, height.x);
	EmitVertex();
	
	gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);
	UV = vec2(length.x, height.x);
	EmitVertex();
	
	EndPrimitive();
}