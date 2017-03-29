// billboard geometry shader

#version 400

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 u_mvp;
uniform mat4 u_mv;
uniform mat4 u_proj;

// Input from Vertex shader
in VertexData
{
	vec3 normal;
	vec2 texCoord;
	vec4 colour;
	vec3 posEye;
} vIn[]; // array size = num vertices in primitve

// Output from geometry shader
//out VertexData
//{
//	vec3 normal;
	vec2 texCoord;
//	vec4 colour;
//	vec3 posEye;
//} vOut; // array size depends on max_vertices


// Creates a quad of specified size around point p
void PointToQuadBillboarded(vec4 p, float size)
{
	float halfSize = size * 0.5;

	vec4 pEye = u_mv * p;
	vec4 topLeft = vec4(pEye.xy + vec2(-halfSize, halfSize), pEye.z, 1.0);
	vec4 botLeft = vec4(pEye.xy + vec2(-halfSize, -halfSize), pEye.z, 1.0);
	vec4 topRight = vec4(pEye.xy + vec2(halfSize, halfSize), pEye.z, 1.0);
	vec4 botRight = vec4(pEye.xy + vec2(halfSize, -halfSize), pEye.z, 1.0);

	gl_Position = u_proj * topRight;
	texCoord = vec2(1.0, 1.0);
	EmitVertex();
	
	gl_Position = u_proj * topLeft;
	texCoord = vec2(0.0, 1.0);
	EmitVertex();
	
	gl_Position = u_proj * botRight;
	texCoord = vec2(1.0, 0.0);
	EmitVertex();

	
	gl_Position = u_proj * botLeft;
	texCoord = vec2(0.0, 0.0);
	EmitVertex();

	EndPrimitive();
	
}

void main()
{
	PointToQuadBillboarded(gl_in[0].gl_Position, 1.0);
}