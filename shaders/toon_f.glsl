#version 400
//Robert Savaglio, 100591436

uniform vec4 u_lightPos;
uniform vec4 u_lightTwo;
uniform vec4 u_spotDir;
uniform vec4 u_controls;
uniform vec4 u_dimmers;
uniform vec4 u_shine;
uniform vec4 u_emissiveLight;

uniform sampler2D u_diffuseTex;
uniform sampler2D u_specularTex;
uniform sampler2D u_toonRamp;
uniform sampler2D u_shadowMap;

uniform vec4 u_transparency;

in VertexData
{
	vec3 normal;
	vec2 texCoord;
	vec4 colour;
	vec3 posEye;
	vec3 posInLight; // light space (shadow map)
} vIn;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragDepth;


float calculateCircleOfConfusion()
{
	// S2 is the distance this pixel is from the camera
	// This value is computed for you in the default_v vertex shader
	
	float depthEye = vIn.posEye.z;
	float S2 = abs(depthEye);

	//// CALCULATE PIXEL BLURRINESS HERE (SEE LAB DOCUMENT PART 1)
	float A = 0.7062;
	float f = 0.0303;
	float S1 = 20.0;

	float c = A * (abs(S2 - S1) / S2) * (f / (S1 - f));

	float sensorHeight = 0.024;

	float percentOfSensor = c / sensorHeight;

	float blurFactor = clamp(percentOfSensor, 0.0, 1.0);
	
	return blurFactor;
}

void main()
{
	vec4 diffuseColour = texture2D(u_diffuseTex, vIn.texCoord.st);
	vec4 specColour = texture2D(u_specularTex, vIn.texCoord.st);

	//Diffuse Component
	vec3 L = normalize(u_lightPos.xyz - vIn.posEye);
	vec3 N = normalize(vIn.normal);
	float diffuse = max(0.0, dot(N, L));

	//diffuse = texture(u_toonRamp, vec2(diffuse)).x;

	//Specular Component
	vec3 V = normalize(-vIn.posEye);
	vec3 R = normalize(2.0 * N * diffuse - L);
	float specular = pow(max(dot(V, R), 0.0), u_shine.x);

	//Rim Component
	float rim = (1.0 - max(dot(V, N), 0.0));

	// Toon Lighting
	if (diffuse <= 0) specular = 0;
	else if (diffuse <= 0.30) diffuse = 0.30;
	else if (diffuse <= 0.60) diffuse = 0.60;

	vec3 deskLamp = vec3(
		+ vec3(diffuse * diffuseColour.rgb * u_controls.y)                      // Diffuse for Desk Lamp
		+ vec3(specular * specColour.rgb * u_controls.z)				        // Specular for Desk Lamp
		);

	//Calculations for Spot Light
	vec3 spotDirection = normalize(u_spotDir.xyz);
	float cosDirection = max(dot(L, spotDirection), 0.0);

	deskLamp = vec3(deskLamp * smoothstep(u_dimmers.z, u_dimmers.w, cosDirection)); // spotlight calculations

	//shadow component (desk lamp)
	vec3 shadowCoord = vIn.posInLight;
	shadowCoord = shadowCoord * 0.5 + 0.5;
	float bias = 0.007;
	vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0);
	
	float shadow = 0;

	//calculate and blur the shadows
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(u_shadowMap, shadowCoord.xy + vec2(x, y) * texelSize).r;
			shadow += shadowCoord.z - bias < pcfDepth ? 1.0 : 0.4;
		}
	}
	shadow /= 9.0;

	deskLamp *= shadow;

	//Diffuse Component for Room Light
	vec3 L2 = normalize(u_lightTwo.xyz - vIn.posEye);
	float diffuse2 = max(0.0, dot(N, L2));


	//Specular For Room Light
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
		+ u_emissiveLight.xyz	   
		);		
	FragColor.w = u_transparency.x;

	FragDepth = vec4(calculateCircleOfConfusion());

}