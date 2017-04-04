#version 400

//uniform sampler2D u_tex;

// Fragment Shader Inputs
//in VertexData
//{
	//vec3 normal;
	//vec2 texCoord;
	//vec4 colour;
	//vec3 posEye;
//} vIn;

layout(location = 0) out vec4 FragColor;

void main()
{
	//vec2 uv = (vIn.texCoord).xy;
	FragColor = vec4(0.0, 0.0, 1.0, 1.0);
}
