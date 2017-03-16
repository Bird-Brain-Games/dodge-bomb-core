#include "particle.h"
#include "GLM\gtc\matrix_transform.hpp"
Generator::Generator() :
renderProg("particleRender"), updateProg("particleUpdate")
{
	initialized = false;
	iCurReadBuffer = 0;
}

bool Generator::init()
{
	if (initialized) return false;

	const char* sVaryings[PARTICLES_ATTRIBUTES] =
	{
		"vpositionOut",
		"vVelocityOut",
		"vColorOut",
		"fLifeTimeOut",
		"fSizeOut",
		"iTypeOut",
	};

	// the shader that updates our particles
	//loads shaders
	vertUpdate.loadShaderFromFile("Shaders/particleUpdate.vert", GL_VERTEX_SHADER);
	geomUpdate.loadShaderFromFile("Shaders/particleUpdate.geom", GL_GEOMETRY_SHADER);

	//makes shader program
	updateProg.attachShader(vertUpdate);
	updateProg.attachShader(geomUpdate);

	//setup the writing to another buffer.
	for (int i = 0; i < PARTICLES_ATTRIBUTES; i++)
	{
		glTransformFeedbackVaryings(updateProg.getHandle(), 6, sVaryings, GL_INTERLEAVED_ATTRIBS);
	}
	updateProg.linkProgram();


	//the shader that renders our particles

	vertRender.loadShaderFromFile("Shaders/particleDraw.vert", GL_VERTEX_SHADER);
	fragRender.loadShaderFromFile("Shaders/particleDraw.frag", GL_FRAGMENT_SHADER);
	geomRender.loadShaderFromFile("Shaders/particleDraw.geom", GL_GEOMETRY_SHADER);

	renderProg.linkProgram();

	//create the necessary buffers for our two shaders.
	glGenTransformFeedbacks(1, &transformBuffer);
	glGenQueries(1, &uiQuery);

	glGenBuffers(2, uiParticleBuffer);
	glGenVertexArrays(2, uiVAO);

	Particle initial;
	initial.type = GENERATOR;

	for (int i = 0; i < 2; i++)
	{
		glBindVertexArray(uiVAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, uiParticleBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * MAX_PARTICLES, NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Particle), &initial);

		for (int i = 0; i < PARTICLES_ATTRIBUTES; i < 0)
			glEnableVertexAttribArray(i);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)0); // position

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)12); // velocity

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)24); // colour

		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)36); // lifetime

		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)40); // size

		glVertexAttribPointer(5, 1, GL_INT, GL_FALSE, sizeof(Particle), (const GLvoid*)44); // type
	}
	iCurReadBuffer = 0;
	iNumParticles = 1;

	initialized = true;

	return true;
}

void Generator::SetGeneratorProperties(glm::vec3 a_vGenPosition, glm::vec3 a_vGenVelocityMin, glm::vec3 a_vGenVelocityMax, glm::vec3 a_vGenGravityVector, glm::vec3 a_vGenColor, float a_fGenLifeMin, float a_fGenLifeMax, float a_fGenSize, float fEvery, int a_iNumToGenerate)
{
	vGenPosition = a_vGenPosition;
	vGenVelocityMin = a_vGenVelocityMin;
	vGenVelocityMax = a_vGenVelocityMax - a_vGenVelocityMin;

	vGenGravityVector = a_vGenGravityVector;
	vGenColour = a_vGenColor;
	fGenSize = a_fGenSize;

	fGenLifeMin = a_fGenLifeMin;
	fGenLifeMax = a_fGenLifeMax - a_fGenLifeMin;

	fNextGenerationTime = fEvery;
	fElapsedTime = 0.8f;

	iNumToGenerate = a_iNumToGenerate;
}

void Generator::updateParticles(float timePassed)
{
	if (!initialized) return;

	updateProg.bind();
	updateProg.sendUniformInt("fTimePassed", timePassed);
	updateProg.sendUniformVec3("vGenPosition", vGenPosition);
	updateProg.sendUniformVec3("vGenVelocityMin", vGenVelocityMin);
	updateProg.sendUniformVec3("vGenVelocityMax", vGenVelocityMax);
	updateProg.sendUniformVec3("vGenColour", vGenColour);
	updateProg.sendUniformVec3("vGenGravity", vGenGravityVector);

	updateProg.sendUniformInt("fGenLifeMin", fGenLifeMin);
	updateProg.sendUniformInt("fGenLifeMax", fGenLifeMax);

	updateProg.sendUniformInt("fGenSize", fGenSize);
	updateProg.sendUniformInt("iNumToGenerate", 0);

	fElapsedTime += timePassed;

	float fRandom = float(rand() % (RAND_MAX + 1)) / float(RAND_MAX);
	float fRandom2 = float(rand() % (RAND_MAX + 1)) / float(RAND_MAX);
	float fRandom3 = float(rand() % (RAND_MAX + 1)) / float(RAND_MAX);
	float fMin = -10.0f; float fAdd = 20.0f;

	if (fElapsedTime > fNextGenerationTime)
	{
		updateProg.sendUniformInt("iNumToGenerate", iNumToGenerate);
		fElapsedTime -= fNextGenerationTime;

		glm::vec3 vRandomSeed = glm::vec3(fMin + fAdd*fRandom, fMin + fAdd*fRandom2, fMin + fAdd*fRandom3);
		updateProg.sendUniformVec3("vRandomSeed", vRandomSeed);

	}
	glEnable(GL_RASTERIZER_DISCARD);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformBuffer);

	glBindVertexArray(uiVAO[iCurReadBuffer]);
	glEnableVertexAttribArray(1);

	glBindBufferBase(GL_TRANSFORM_FEEDBACK, 0, uiParticleBuffer[1 - iCurReadBuffer]);

	glBeginQuery(GL_TRANSFORM_FEEDBACK_BUFFER, uiQuery);
	glGetQueryObjectiv(uiQuery, GL_QUERY_RESULT, &iNumParticles);

	iCurReadBuffer = 1 - iCurReadBuffer;
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

void Generator::renderParticles()
{
	if (!initialized) return;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(0);

	glDisable(GL_RASTERIZER_DISCARD);
	renderProg.bind();
	renderProg.sendUniformMat4("Matrices.mProj", matProjection);
	renderProg.sendUniformMat4("Matrices.mProj", matView);
	renderProg.sendUniformVec3("Matrices.mProj", vQuad1);
	renderProg.sendUniformVec3("Matrices.mProj", vQuad2);
	renderProg.sendUniformInt("u_tex", 0);

	glBindVertexArray(uiVAO[iCurReadBuffer]);
	glDisableVertexAttribArray(1); // disables velocity since we dont use it

	glDrawArrays(GL_POINTS, 0, iNumParticles);

	glDepthMask(1);
	glDisable(GL_BLEND);

}

void Generator::clearAllParticles()
{

}
bool Generator::releaseGenerator()
{
	return false;
}

void Generator::setMatrices(glm::mat4* proj, glm::vec3 vEye, glm::vec3 vView, glm::vec3 vUp)
{
	matProjection = glm::lookAt(vEye, vView, vUp);
	vView = vView - vEye;
	vView = glm::normalize(vView);
	vQuad1 = glm::cross(vView, vUp);
	vQuad1 = glm::normalize(vQuad1);

	vQuad2 = glm::cross(vView, vUp);
	vQuad2 = glm::normalize(vQuad2);


}


