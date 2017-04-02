// billboard geometry shader

#version 400

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 u_mvp;
uniform mat4 u_mv;
uniform mat4 u_proj;
uniform float u_size;

// Input from Vertex shader
in VertexData
{
	vec3 normal;
	vec2 texCoord;
	vec4 colour;
	vec3 posEye;
} vIn[]; // array size = num vertices in primitve

out GeomData
{
	vec3 normal;
	vec2 texCoord;
	vec3 posEye;
} gOut;



uniform vec2 dimensions; // the size of our atlas (relative to number of elements)

// Creates a quad of specified size around point p
void PointToQuadBillboarded(vec4 p, float size)
{
	vec2 uvSize = vec2(1.0) / dimensions;
	
	vec2 length = vec2(uvSize.x * vIn[0].texCoord.x, uvSize.x * (vIn[0].texCoord.x+1));
	vec2 height = vec2(uvSize.y * vIn[0].texCoord.y, uvSize.y * (vIn[0].texCoord.y+1));
	
	
	float halfSize = size * 0.5;

	vec4 pEye = u_mv * p;
	vec4 topLeft = vec4(pEye.xy + vec2(-halfSize, halfSize), pEye.z, 1.0);
	vec4 botLeft = vec4(pEye.xy + vec2(-halfSize, -halfSize), pEye.z, 1.0);
	vec4 topRight = vec4(pEye.xy + vec2(halfSize, halfSize), pEye.z, 1.0);
	vec4 botRight = vec4(pEye.xy + vec2(halfSize, -halfSize), pEye.z, 1.0);

	gl_Position = u_proj * topRight;
	gOut.posEye = topRight.xyz;
	gOut.texCoord = vec2(length.y, height.y);
	gOut.normal = (p - topRight).xyz;
	EmitVertex();
	
	gl_Position = u_proj * topLeft;
	gOut.posEye = topLeft.xyz;
	gOut.texCoord = vec2(length.x, height.y);
	gOut.normal = (p - topLeft).xyz;
	EmitVertex();
	
	gl_Position = u_proj * botRight;
	gOut.posEye = botRight.xyz;
	gOut.texCoord = vec2(length.y, height.x);
	gOut.normal = (p - botRight).xyz;
	EmitVertex();

	
	gl_Position = u_proj * botLeft;
	gOut.posEye = botLeft.xyz;
	gOut.texCoord = vec2(length.x, height.x);
	gOut.normal = (p - botLeft).xyz;
	EmitVertex();

	EndPrimitive();
	
}

void main()
{
	PointToQuadBillboarded(gl_in[0].gl_Position, u_size);
}