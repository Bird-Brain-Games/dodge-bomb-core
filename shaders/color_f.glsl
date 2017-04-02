#version 400

// FUTURE GOALS
// take in arrays of lookups and add them together

uniform sampler2D u_tex;

uniform sampler3D u_lookup;
uniform float u_lookupSize;
uniform float u_mixAmount;

// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec2 texCoord;
	vec4 colour;
	vec3 posEye;
} vIn;

in vec2 texCoord;	// From Geom shader

layout(location = 0) out vec4 FragColor;

void main()
{
	vec4 rawColor = texture(u_tex, texCoord);

	vec3 scale; 
	vec3 offset;
	vec3 applyLut;

	// Apply color correction
	scale = vec3((u_lookupSize - 1.0) / u_lookupSize);
	offset = vec3(1.0 / (2.0 / u_lookupSize));
	applyLut = texture(u_lookup, scale * rawColor.xyz + offset).rgb;
	vec3 lookupColor = mix(rawColor.xyz, applyLut, u_mixAmount);

	// Apply color corrections
	FragColor.xyz = lookupColor; //lookupColor + rawColor.xyz;
	FragColor.w = 1.0;
}