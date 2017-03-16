#pragma once
#include "GLM\glm.hpp"
#include "GLEW/glew.h"
#include "ShaderProgram.h"

#define PARTICLES_ATTRIBUTES 6
#define MAX_PARTICLES 100000

#define GENERATOR 0
#define NORMAL 1



//contains all the data for a particle
struct Particle
{
	glm::vec3 pos;
	glm::vec3 velocity;
	glm::vec3 colour; // replace with sprite
	float life;
	float size;
	int type;
};

class Generator
{
public:
	bool init();

	void renderParticles();
	void updateParticles(float timePassed);

	void SetGeneratorProperties(glm::vec3 a_vGenPosition, glm::vec3 a_vGenVelocityMin, glm::vec3 a_vGenVelocityMax, glm::vec3 a_vGenGravityVector, glm::vec3 a_vGenColor, float a_fGenLifeMin, float a_fGenLifeMax, float a_fGenSize, float fEvery, int a_iNumToGenerate);

	void clearAllParticles();
	bool releaseGenerator();

	void setMatrices(glm::mat4* proj, glm::vec3 vEye, glm::vec3 vView, glm::vec3 vUp);


	Generator();

private:
	bool initialized;

	GLuint transformBuffer;

	GLuint uiParticleBuffer[2];
	GLuint uiVAO[2];

	GLuint uiQuery;
	GLuint uiTexture;

	int iCurReadBuffer;
	int iNumParticles;

	glm::mat4 matProjection, matView;
	glm::vec3 vQuad1, vQuad2;

	float fElapsedTime;
	float fNextGenerationTime;

	glm::vec3 vGenPosition;
	glm::vec3 vGenVelocityMin, vGenVelocityMax;
	glm::vec3 vGenGravityVector;
	glm::vec3 vGenColour;

	float fGenLifeMin, fGenLifeMax;
	float fGenSize;

	int iNumToGenerate;

	Shader vertRender, fragRender, geomRender;
	Shader vertUpdate, fragUpdate, geomUpdate;
	ShaderProgram renderProg;
	ShaderProgram updateProg;
};