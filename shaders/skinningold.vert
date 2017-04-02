#version 330

layout (location = 4) in vec3 inPosition;//our objects position
layout (location = 5) in vec2 vertexUV;
layout (location = 6) in vec3 normal;
layout (location = 8) in vec4 bones;//The bones that are affecting this vertex
layout (location = 9) in vec4 weights; //The weights for the bones we are using.

uniform mat4 mvm; // modelview matrix
uniform mat4 prm; // projection matrix

uniform mat4 localTransform;

uniform int boneCount;
uniform highp mat4 BoneMatrixArray[28];

//out vec3 colour;
out vec3 normal0;
out vec2 UV;

void main()
{
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
				
				position += boneMatrix * vec4(inPosition, 1.0) * boneWeights.x;
				normalM += boneMatrix * vec4(normal, 0.0) * boneWeights.x;
			}
		}
		boneIndex = boneIndex.yzwx;
		boneWeights = boneWeights.yzwx;
	}
	normal0 = normalize(normal0).xyz;
	UV = vertexUV;
	
	gl_Position = prm * mvm * (localTransform * position);
	//colour = vec3(0.0, 0.0, 1.0);
}