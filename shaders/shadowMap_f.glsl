#version 400
// Fragment Shader Inputs
layout(location = 0) out vec4 FragColor;


void main()
{
	FragColor.rgb = vec3(gl_FragCoord.z);
	FragColor.a = 1.0;
}