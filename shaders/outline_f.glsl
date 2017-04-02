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
layout(location = 1) out vec4 FragDepth;
void main()
{
	//////////////////////////////////////
	/// SET FRAGCOLOR TO UNIFORM COLOR ///
	//////////////////////////////////////
	FragColor = u_outlineColour;
	FragColor.w = 1.0;

	FragDepth = vec4(0.0);
}