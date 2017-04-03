#version 400

// Vertex Shader Inputs
// These are the attributes of the vertex
layout(location = 0) in vec3 vIn_vertex;
layout(location = 1) in vec3 vIn_normal;
layout(location = 2) in vec2 vIn_uv;
layout(location = 3) in float size;

// Uniforms
// Constants throughout the entire pipeline
// These values are sent from C++ (glSendUniform*)
uniform mat4 u_mvp;
uniform mat4 u_mv;
uniform vec4 u_colour;

out VertexData
{
	vec3 normal;
	vec2 texCoord;
	float size;
} vOut; 

void main() 
{
	vOut.texCoord = vIn_uv;
	vOut.normal = vIn_normal;
	vOut.size = size;
	gl_Position = vec4(vIn_vertex, 1.0);
}