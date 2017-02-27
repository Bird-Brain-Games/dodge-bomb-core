#version 400

uniform vec4 u_lightPos;
uniform vec4 u_colour;

uniform sampler2D u_diffuseTex;
uniform sampler2D u_specularTex;

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
	vec4 diffuseColour = texture2D(u_diffuseTex, vIn.texCoord.st);
	vec4 specColour = texture2D(u_specularTex, vIn.texCoord.st);

	//Diffuse Component
	vec3 L = normalize(u_lightPos.xyz - vIn.posEye);
	vec3 N = normalize(vIn.normal);

	float diffuse = max(0.0, dot(N, L));

	if (diffuse <= 0.00) diffuse = 0.00;
	else if (diffuse <= 0.25) diffuse = 0.25;
	else if (diffuse <= 0.50) diffuse = 0.50;
	else if (diffuse <= 0.75) diffuse = 0.75;
	else diffuse = 1.00;

	//Specular Component
	vec3 V = normalize(-vIn.posEye);
	vec3 R = normalize(2.0 * N * diffuse - L);
	float specular = pow(max(dot(V, R), 0.0), 1.0);

	if (diffuse <= 0) specular = 0;


	FragColor.rgb = vec3(
		  vec3(0.1, 0.1, 0.1)
		+ vec3(diffuse * diffuseColour.rgb)         // Diffuse
		+ vec3(specular * specColour.rgb));       // Specular
	FragColor.w = 1.0;
}