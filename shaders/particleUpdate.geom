#version 330

layout(points) in;
layout(points) out;
layout(max_vertices = 40) out;

in vec3  vPositionPass[];
in vec3  vVelocityPass[];
in vec3  vColorPass[];
in float fLifeTimePass[];
in float fSizePass[];
in int   iTypePass[];

out vec3  vPositionOut;
out vec3  vVelocityOut;
out vec3  vColorOut;
out float fLifeTimeOut;
out float fSizeOut;
out int   iTypeOut;

uniform vec3 vGenPos;
uniform vec3 vGenGravityVector;
uniform vec3 vGenVelocityMin;
uniform vec3 vGenVelocityRange;

uniform vec3 vGenColor;
uniform float fGenSize;

uniform float fGenLifeMin, fGenLifeRange;
uniform float fTimePassed;

uniform vec3 vRandomSeed;
vec3 vLocalSeed;

uniform int iNumToGenerate;

float randZeroOne()
{
	uint n = floatBitToUnit(vLocalSeed.y * 214013.0 + vLocalSeed.x * 2531011.0 + vLocalSeed.z * 141251.0);
	n = n * (n * n * 15731u +  789221u);
	n = (n >> 9u) | 0x3F800000u;
	
	float fRes = 2.0 - uintBitsToFloat(n);
	vLocalSeed = vec3(vLocalSeed.x + 147158.0 * fRes, vLocalSeed.y * fRes + 415161.0 * fRes, vLocalSeed.z + 324154.0*fRes);
	return fRes;
}

void main()
{
	vLocalSeed = vRandomSeed;
	
	vPositionOut = vPositionPass[0];
	vVelocityOut = vVelocityPass[0];
	
	//this can be optimized to remove the if statement. (dynamic bad)
	if(iTypePass[0] != 0) vPositionOut += vVelocityOut * fTimePassed;
	if(iTypePass[0] != 0) vVelocityOut += vGenGravityVector * fTimePassed;
		
	vColorOut = vColorPass[0];
	fLifeTimeOut = fLifeTimePass[0] - fTimePassed;
	fSizeOut = fSizePass[0];
	iTypeOut = iTypePass[0];
	
	//this can be optimized to remove the if statement (dynamic bad)
	if (iTypeOut == 0)
	{
	
		emitVertex();
		//this cannot be optimized but since its based on a uniform the compiler
		//Can optimize it to an acceptable level (static okay)
		for (int i = 0; i < iNumToGenerate; i++)
		{
			vPositionOut = vGenPos;
			vVelocityOut = vGenVelocityMin + vec3(vGenVelocityRange.x * randZeroOne(), vGenVelocityRange.y * randZeroOne(), vGenVelocityRange.z * randZeroOne());
			vColorOut = vGenColor;
			fLifeTimeOut = fGenLifeMin + fGenLifeRange * randZeroOne();
			iTypeOut = 1;
			EmitVertex();
			EndPrimitive();
			
		}
	}
	else if (fLifeTimeOut > 0.0)
	{
		EmitVertex();
		EndPrimitive();
	}
}












