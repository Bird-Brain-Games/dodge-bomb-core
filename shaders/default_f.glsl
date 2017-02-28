#version 400
//Robert Savaglio, 100591436

uniform vec4 u_lightPos;
uniform vec4 u_controls;
uniform vec4 u_shine;
uniform sampler2D u_diffuseTex;
uniform sampler2D u_specularTex;

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


	//Specular Component
	vec3 V = normalize(-vIn.posEye);
	vec3 R = normalize(2.0 * N * diffuse - L);
	float specular = pow(max(dot(V, R), 0.0), u_shine.x);

	//Rim Component
	float rim = (1.0 - max(dot(V, N), 0.0));



	if (diffuse <= 0) specular = 0;

	FragColor.rgb = vec3(
		  vec3(u_controls.x)											        // Ambient
		+ vec3(diffuse * diffuseColour.rgb * u_controls.y)                      // Diffuse
		+ vec3(specular * specColour.rgb * u_controls.z)				        // Specular
		+ vec3(smoothstep(0.8, 1.0, rim) * vec3(1.0, 0.6, 0.4) * u_controls.w)  // Rim		   
		);		
	FragColor.w = 1.0;
}