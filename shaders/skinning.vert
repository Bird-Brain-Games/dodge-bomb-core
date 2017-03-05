
#version 330

layout (location = 0) in vec3 vIn_vertex;//our objects position
layout (location = 1) in vec3 vIn_normal;
layout (location = 2) in vec3 vIn_uv;
layout (location = 8) in vec4 bones;//The bones that are affecting this vertex
layout (location = 9) in vec4 weights; //The weights for the bones we are using.

uniform mat4 u_mvp; // model view projection matrix
uniform mat4 u_mv; // model view matrix



uniform int boneCount;
uniform highp mat4 BoneMatrixArray[28];


out VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 posEye;
} vOut;


//out vec3 colour;


void main()
{

	vOut.texCoord = vIn_uv;
	vOut.colour = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	

	mediump ivec4 boneIndex = ivec4(bones);
	mediump vec4 boneWeights = weights;
	vec4 normalM = vec4(0.0, 0.0, 0.0, 0.0);
	
	highp vec4 position = vec4(0.0, 0.0, 0.0, 0.0);
	for (lowp int i = 0; i < 4; i++)
	{
		if (boneIndex.x < boneCount && boneIndex.x >= 0)
		{
			if (boneWeights.x > 0)
			{
				highp mat4 boneMatrix = BoneMatrixArray[boneIndex.x];
				
				position += boneMatrix * vec4(vIn_vertex, 1.0) * boneWeights.x;
				normalM += boneMatrix * vec4(vIn_normal, 0.0) * boneWeights.x;
			}
		}
		boneIndex = boneIndex.yzwx;
		boneWeights = boneWeights.yzwx;
	}
	
	vOut.normal = (u_mv* normalM).xyz;
	vOut.posEye = (u_mv* position).xyz;
	
	gl_Position = u_mvp * vec4(vIn_vertex, 1.0);
	//colour = vec3(0.0, 0.0, 1.0);
}