#version 400

uniform sampler2D u_tex;
uniform vec3 colour;
// Fragment Shader Inputs
in GeomData
{
	vec3 normal;
	vec2 texCoord;
	vec3 posEye;
} gIn;

uniform vec4 u_lightPos;
uniform vec4 u_lightTwo;
uniform vec4 u_spotDir;
uniform vec4 u_controls;
uniform vec4 u_dimmers;
uniform vec4 u_shine;
uniform vec4 u_emissiveLight;

uniform float mixer;

layout(location = 0) out vec4 FragColor;

void main()
{
	vec2 uv = gIn.texCoord.xy;
	vec4 textureColour = texture(u_tex, uv);
	vec3 newColour = textureColour.xyz;
//	mix(textureColour.xyz, colour, mixer);
	
	vec3 L = normalize(u_lightPos.xyz - gIn.posEye);
	vec3 N = normalize(gIn.normal);
	float diffuse = max(0.0, dot(N, L));
	
	vec3 V = normalize(-gIn.posEye);
	vec3 R = normalize(2.0 * N * diffuse - L);
	float specular = pow(max(dot(V, R), 0.0), u_shine.x);
	
	if (diffuse <= 0) specular = 0;
	else if (diffuse <= 0.30) diffuse = 0.30;
	else if (diffuse <= 0.60) diffuse = 0.60;
	
	vec3 deskLamp = vec3(
		+ vec3(diffuse * newColour * u_controls.y)                      // Diffuse for Desk Lamp
		+ vec3(specular * newColour * u_controls.z)				        // Specular for Desk Lamp
		);
		
	vec3 spotDirection = normalize(u_spotDir.xyz);
	float cosDirection = max(dot(L, spotDirection), 0.0);
	
	deskLamp = vec3(deskLamp * smoothstep(u_dimmers.z, u_dimmers.w, cosDirection)); // spotlight calculations
	
	//Diffuse Component for Room Light
	vec3 L2 = normalize(u_lightTwo.xyz - gIn.posEye);
	float diffuse2 = max(0.0, dot(N, L2));


	//Specular For Room Light
	vec3 R2 = normalize(2.0 * N * diffuse2 - L2);
	float specular2 = pow(max(dot(V, R2), 0.0), u_shine.x);

	vec3 roomLight = vec3(
		+ vec3(diffuse2 * newColour * u_controls.y)                      // Diffuse for room light
		+ vec3(specular2 * newColour * u_controls.z)				         // Specular for room light   
		);

	vec3 lights = (deskLamp * u_dimmers.x + roomLight * u_dimmers.y);

	FragColor.rgb = vec3(
		  vec3(u_controls.x)											        // Ambient
		+ vec3(lights)
		+ u_emissiveLight.xyz	   
		);
	
	FragColor.rgb = mix(FragColor.xyz, colour, mixer);
	
	if (textureColour.w > 0.0)
		textureColour.w = 1.0;
		
	FragColor.w = textureColour.w;
}
