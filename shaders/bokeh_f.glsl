//Robert Savaglio, 100591436
#version 400

uniform sampler2D u_depth;
uniform sampler2D u_tex;
uniform sampler2D u_texD;
uniform vec4 u_cameraParams;	// x = filterAngle, y = aspect ratio

// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 posEye;
} vIn;

in vec2 texCoord;

layout(location = 0) out vec4 FragColor;

// More samples means higher quality but higher computational cost
const int NUM_SAMPLES = 14;

// Note: Calculating the offsets every pixel is *very* inefficient
// The ideal way is to calculate the offsets CPU side and send them as Uniform values
void calculateOffsets(float angle, out vec2 offsets[NUM_SAMPLES])
{
	
	float radius = 0.5;
	/// TODO: Calculate Bokeh filter offsets here (SEE LAB DOCUMENT PART 2)
	vec2 pt = vec2(radius * cos(angle), radius * sin(angle));

	pt.x /= u_cameraParams.y;

	for(int i = 0; i < NUM_SAMPLES; i++)
	{
			float t = i / (NUM_SAMPLES - 1.0);
			offsets[i] = mix(-pt, pt, t);  
	}
}

vec4 bokeh(vec2 offsets[NUM_SAMPLES])
{
	/// TODO: Implement Bokeh filter here (SEE LAB DOCUMENT PART 2)
	float bleedingBias = 0.02;
	float bleedingMult = 1.0;

	vec4 centerPixel = texture(u_tex, texCoord);
	vec4 centerDepth = texture(u_depth, texCoord);
	vec4 centerCircle = texture(u_texD, texCoord);

	vec4 colour = vec4(0.0, 0.0, 0.0, 0.0);
	float totalWeight = 0.0;
	
	for(int t = 0; t < NUM_SAMPLES; t++)
	{
	vec2 offset = offsets[t];
			vec2 sampleCoords = texCoord + offset * centerCircle.a;

			vec4 samplePixel = texture(u_tex, sampleCoords);
			vec4 sampleDepth = texture(u_depth, sampleCoords);
			vec4 sampleCircle = texture(u_texD, sampleCoords);

			float weight = sampleDepth.a < centerDepth.a ? sampleCircle.a * bleedingMult : 1.0;
			weight = (centerCircle.a > sampleCircle.a + bleedingBias) ? weight : 1.0;
			weight = clamp(weight, 0.0, 1.0);

			colour += samplePixel * weight;
			totalWeight += weight;
	}

	return colour / totalWeight;
}

void main()
{
	//float angle = u_cameraParams.z * (3.14159 / 180.0);

	float angle = u_cameraParams.x;
	// Calculate offsets
	vec2 offsets[NUM_SAMPLES];
	calculateOffsets(angle, offsets);

	// Perform Bokeh filter
	FragColor = bokeh(offsets);
//	FragColor.a = 1.0;
}