/*
	Vertex Shader

	Uses GL's MVP stack to transform the incoming vertex.
*/

#version 330
layout(location = 4) in vec3 vPos;
layout(location = 5) in vec2 texture;
layout(location = 6) in vec3 normal;
layout(location = 7) in vec3 color;

out vec3 normal0;
out vec2 texCoord;
out vec3 color0;

//lighting outs
out vec3 lightDir;

uniform mat4 mvm; // modelview matrix
uniform mat4 prm; // projection matrix

uniform mat4 localTransform;

uniform vec3 lightPosition;

void main()
{
	color0 = color;
	normal0 = mat3(mvm) * mat3(localTransform) * normal;
	texCoord = texture;

	vec4 localSpace = localTransform * vec4(vPos, 1.0);
	vec4 modelViewSpace = mvm * localSpace;
	vec4 worldSpace = prm * modelViewSpace;

	lightDir = normalize(lightPosition - modelViewSpace.xyz);

	gl_Position = worldSpace;	//every vert shader must set gl_Position
}



