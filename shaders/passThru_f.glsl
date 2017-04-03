/*
	Fragment Shader

	Sets the fragment colour
*/

#version 330

in vec3 normal0;
in vec2 texCoord;
in vec3 color0;

in vec3 lightDir;

uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;

uniform float cloudRamp;

void main()
{
	vec4 color1 = texture2D(tex1, texCoord.st);
	vec4 color2 = texture2D(tex2, texCoord.st);
	vec4 color3 = texture2D(tex3, texCoord.st) * cloudRamp;

	vec3 L = lightDir;
	float lightIntensity = clamp(dot(normal0, L), 0.0, 1.0);

	color1 = mix(color1, vec4(0.8, 0.8, 0.8, 1.0), color3.r);
	color2 = mix(color2, vec4(0.1, 0.1, 0.1, 1.0), color3.r);

	vec4 color = mix(color1, color2, 1.0 - lightIntensity);

	color.w = 1.0;

	gl_FragColor = color;
}