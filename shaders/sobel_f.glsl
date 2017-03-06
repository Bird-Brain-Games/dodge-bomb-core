#version 400

//////////////////////
/// CREATE UNIFORMS ///
///////////////////////
uniform vec4 u_outlineColour;


// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 posEye;
} vIn;

layout(location = 0) out vec4 FragColor;

void main()
{
	//////////////////////////////////////
	/// SET FRAGCOLOR TO UNIFORM COLOR ///
	//////////////////////////////////////
	FragColor = u_outlineColour;
}