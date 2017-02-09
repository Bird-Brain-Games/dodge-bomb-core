#version 400

uniform sampler2D u_rgb;
uniform sampler2D u_normals;
uniform vec4 u_texelSize; // (1.0 / windowWidth, 1.0 / windowHeight)

// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 posEye;
} vIn;

layout(location = 0) out vec4 FragColor;

float horizontalSobel(sampler2D tex) // returns gx
{
	vec2 uv = vIn.texCoord.xy;
	vec2 offsetCoordinates[6];

	// left column
	offsetCoordinates[0] = vec2(-u_texelSize.x, u_texelSize.y) + uv;	// top left
	offsetCoordinates[1] = vec2(-u_texelSize.x, 0.0) + uv;				// mid left
	offsetCoordinates[2] = vec2(-u_texelSize.x, -u_texelSize.y) + uv;	// bot left

	// Right column
	offsetCoordinates[3] = vec2(u_texelSize.x, u_texelSize.y) + uv;		// top right
	offsetCoordinates[4] = vec2(u_texelSize.x, 0.0) + uv;				// mid right
	offsetCoordinates[5] = vec2(u_texelSize.x, -u_texelSize.y) + uv;	// bot right
	
	float sobel[6];

	// left column in filter
	sobel[0] = -1.0;
	sobel[1] = -2.0;
	sobel[2] = -1.0;

	// Right column in filter
	sobel[3] = 1.0;
	sobel[4] = 2.0;
	sobel[5] = 1.0;

	vec3 gx = vec3(0.0);
	gx += texture(tex, offsetCoordinates[0]).rgb * sobel[0];
	gx += texture(tex, offsetCoordinates[1]).rgb * sobel[1];
	gx += texture(tex, offsetCoordinates[2]).rgb * sobel[2];
	gx += texture(tex, offsetCoordinates[3]).rgb * sobel[3];
	gx += texture(tex, offsetCoordinates[4]).rgb * sobel[4];
	gx += texture(tex, offsetCoordinates[5]).rgb * sobel[5];

	float sqrMag = dot(gx, gx);

	if (sqrMag > 0.5)	// 0.1 = outline threshold, should make uniform value
		return 0.0;
	else
		return 1.0;
}

float verticalSobel(sampler2D tex) // returns gy
{
	vec2 uv = vIn.texCoord.xy;
	vec2 offsetCoordinates[6];

	// left column
	offsetCoordinates[0] = vec2(-u_texelSize.x, u_texelSize.y) + uv;	// top left
	offsetCoordinates[1] = vec2(0.0, u_texelSize.y) + uv;				// top mid
	offsetCoordinates[2] = vec2(-u_texelSize.x, u_texelSize.y) + uv;	// top right

	// Right column
	offsetCoordinates[3] = vec2(-u_texelSize.x, -u_texelSize.y) + uv;	// bot left
	offsetCoordinates[4] = vec2(0.0, -u_texelSize.y) + uv;				// bot mid
	offsetCoordinates[5] = vec2(-u_texelSize.x, -u_texelSize.y) + uv;	// bot right
	
	float sobel[6];

	// left column in filter
	sobel[0] = -1.0;
	sobel[1] = -2.0;
	sobel[2] = -1.0;

	// Right column in filter
	sobel[3] = 1.0;
	sobel[4] = 2.0;
	sobel[5] = 1.0;

	vec3 gy = vec3(0.0);
	gy += texture(tex, offsetCoordinates[0]).rgb * sobel[0];
	gy += texture(tex, offsetCoordinates[1]).rgb * sobel[1];
	gy += texture(tex, offsetCoordinates[2]).rgb * sobel[2];
	gy += texture(tex, offsetCoordinates[3]).rgb * sobel[3];
	gy += texture(tex, offsetCoordinates[4]).rgb * sobel[4];
	gy += texture(tex, offsetCoordinates[5]).rgb * sobel[5];

	float sqrMag = dot(gy, gy);

	if (sqrMag > 0.5)	// 0.1 = outline threshold, should make uniform value
		return 0.0;
	else
		return 1.0;
}

void main()
{
	float sobel = horizontalSobel(u_normals) * verticalSobel(u_normals);

	FragColor = vec4(texture(u_rgb, vIn.texCoord.xy).xyz * sobel, 1.0);
}