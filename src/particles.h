#pragma once

#include "FrameBufferObject.h"
#include "VAO.h"
#include "camera.h"
#include <memory>
#include "Texture2D.h"
#include <math.h>
#include "Texture.h"
#include "material.h"

class ParticleEmmiter // using soa principle
{
private:
	struct Particle
	{
		glm::vec3* position;
		glm::vec3* velocities;
		glm::vec3* acceleration;
		glm::ivec2* location;	// the location in our texture.
		float* duration;	//The total duration it exists for
		float* lives;		//controls how long the particle lives
		float* masses;
	} particles;

public:
	ParticleEmmiter();
	~ParticleEmmiter();

	void initialize(unsigned int numParticles);
	void play();
	void pause();

	void update(float dt, glm::vec3 velocity = glm::vec3(0.0f));

	void draw(Camera);

	void freeMemory();

	glm::vec3 initialPosition;

	glm::vec3 lifeRange;
	glm::vec3 initialForceMin;
	glm::vec3 initialForceMax;
	glm::vec3 initialGravity;
	glm::ivec2 dimensions;	// the dimension of our texture.
	float max;				// the number of textures to cycle through
	float size; // the size of the particles
	float mixer;

	glm::vec3 colour;

	VAO vao;

	bool playing;

	std::shared_ptr<Material> material;

	std::shared_ptr<Texture> texture;

private:
	unsigned int numParticles;
	bool allocated;
};