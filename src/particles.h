#pragma once

#include "FrameBufferObject.h"
#include "VAO.h"
#include "camera.h"
#include <memory>
#include "Texture2D.h"
#include <math.h>
#include "material.h"

class ParticleEmmiter // using soa principle
{
private:
	struct Particle
	{
		glm::vec3* position;
		glm::vec3* velocities;
		glm::vec3* acceleration;
		float* lives;
		float* masses;
	} particles;

public:
	ParticleEmmiter();
	~ParticleEmmiter();

	void initialize(unsigned int numParticles);
	void play();
	void pause();

	void update(float dt);

	void draw(Camera*);

	void freeMemory();

	glm::vec3 initialPosition;

	glm::vec3 lifeRange;
	glm::vec3 initialForceMin;
	glm::vec3 initialForceMax;

	VAO vao;

	bool playing;

	std::shared_ptr<Material> material;

	std::shared_ptr<TTK::Texture2D> texture;

private:
	unsigned int numParticles;
	bool allocated;
};