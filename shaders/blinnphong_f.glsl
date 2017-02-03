/*
	Fragment Shader

	Sets the fragment colour

	// Graham Watson 100522240
	// Robert Savaglio 100591436
*/

#version 330

in vec3 normal0;
in vec2 texCoord;
in vec3 color0;
in vec3 eyePos;

in vec3 lightDir;

uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform float shininess;

void main()
{
	vec3 L = lightDir;
	vec3 N = normalize(normal0);


	// light colors
	vec4 ambient = vec4(0.1, 0.1, 0.1, 1.0);
	vec4 diffuseColor = texture2D(diffuseTex, texCoord.st);
	vec4 specularColor = texture2D(specularTex, texCoord.st);

	// diffuse component
	float lightIntensity = clamp(dot(normal0, L), 0.0, 1.0);
	vec4 diffuse = diffuseColor * lightIntensity;

	// specular component
	vec3 E = normalize(-eyePos);
	vec3 H = normalize(L + E);
	float specularCoefficient = pow( max( dot(N, H), 0.0), shininess);
	vec4 specular = specularColor * specularCoefficient;

	// combine
	vec4 color = ambient + diffuse + specular;
	color.w = 1.0;

	gl_FragColor = color;
}