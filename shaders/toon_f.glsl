#version 400
//Robert Savaglio, 100591436

uniform vec4 u_lightPos;
uniform vec4 u_lightTwo;
uniform vec4 u_spotDir;
uniform vec4 u_controls;
uniform vec4 u_dimmers;
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

	// Toon Lighting
	if (diffuse <= 0) specular = 0;
	else if (diffuse <= 0.15) diffuse = 0.15;
	//else if (diffuse <= 0.30) diffuse = 0.30;
	else if (diffuse <= 0.30) diffuse = 0.30;
	//else if (diffuse <= 0.50) diffuse = 0.50;
	//else if (diffuse <= 0.60) diffuse = 0.60;
	//else if (diffuse <= 0.70) diffuse = 0.70;
	//else if (diffuse <= 0.80) diffuse = 0.80;
	//else if (diffuse <= 0.90) diffuse = 0.90;
	else diffuse = 1.00;

	vec3 deskLamp = vec3(
		+ vec3(diffuse * diffuseColour.rgb * u_controls.y)                      // Diffuse for Desk Lamp
		+ vec3(specular * specColour.rgb * u_controls.z)				        // Specular for Desk Lamp
		);
	
	vec3 spotDirection = normalize(u_spotDir.xyz);
	float cosDirection = max(dot(L, spotDirection), 0.0);
	
	deskLamp = vec3(deskLamp * smoothstep(u_dimmers.z, u_dimmers.w, cosDirection));


	//Diffuse Component for Light 2
	vec3 L2 = normalize(u_lightTwo.xyz - vIn.posEye);
	float diffuse2 = max(0.0, dot(N, L2));


	//Specular For Light 2
	vec3 R2 = normalize(2.0 * N * diffuse2 - L2);
	float specular2 = pow(max(dot(V, R2), 0.0), u_shine.x);

	vec3 roomLight = vec3(
		+ vec3(diffuse2 * diffuseColour.rgb * u_controls.y)                      // Diffuse for room light
		+ vec3(specular2 * specColour.rgb * u_controls.z)				         // Specular for room light   
		);

	vec3 lights = (deskLamp * u_dimmers.x + roomLight * u_dimmers.y);

	FragColor.rgb = vec3(
		  vec3(u_controls.x)											        // Ambient
		+ vec3(lights)
		+ vec3(smoothstep(0.8, 1.0, rim) * vec3(1.0, 1.0, 1.0) * u_controls.w)  // Rim		   
		);		
	FragColor.w = 1.0;
}