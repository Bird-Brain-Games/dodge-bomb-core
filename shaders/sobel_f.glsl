#version 400

//////////////////////
/// CREATE UNIFORMS ///
///////////////////////
uniform vec4 u_outlineColour;
uniform vec4 u_transparency;


// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec2 texCoord;
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
	FragColor.w = u_transparency.x;
}