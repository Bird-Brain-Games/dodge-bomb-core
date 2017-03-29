#include "particles.h"
#include <algorithm>
#include "GLM\gtc\random.hpp"
#include <iostream>
ParticleEmmiter::ParticleEmmiter()
{
	particles.position = nullptr;
	particles.velocities = nullptr;
	particles.lives = nullptr;
	particles.acceleration = nullptr;
	particles.masses = nullptr;
	numParticles = 0;

	playing = false;

}

ParticleEmmiter::~ParticleEmmiter() { freeMemory(); }

void ParticleEmmiter::initialize(unsigned int _numParticles)
{
	if (allocated) freeMemory();

	if (_numParticles)
	{
		particles.position = new glm::vec3[_numParticles];
		particles.velocities = new glm::vec3[_numParticles];
		particles.lives = new float[_numParticles];
		particles.acceleration = new glm::vec3[_numParticles];
		particles.masses = new float[_numParticles];
		memset(particles.lives, 0, sizeof(float) * _numParticles);

		numParticles = _numParticles;
		allocated = true;

			Attribute posAttrib(AttributeLocations::VERTEX, GL_FLOAT, sizeof(glm::vec3), 3, _numParticles, "particles", particles.position);
		vao.addAttribute(posAttrib);
		vao.setPrimitive(GL_POINTS);
		vao.createVAO(GL_DYNAMIC_DRAW);

	}
}

void ParticleEmmiter::play() { playing = true; }
void ParticleEmmiter::pause() { playing = false; }

void ParticleEmmiter::update(float dt)
{
	if (allocated && playing)
	{
		for (int i = 0; i < numParticles; i++)
		{
			glm::vec3* pos = particles.position + i;
			glm::vec3* vel = particles.velocities + i;
			glm::vec3* accel = particles.acceleration + i;
			float* life = particles.lives + i;
			float* mass = particles.masses + i;

			if (*life <= 0)
			{
				*pos = initialPosition;
				(*vel).x = glm::mix(initialForceMin.x, initialForceMax.x, glm::linearRand(0.0f, 1.0f));
				(*vel).y = glm::mix(initialForceMin.y, initialForceMax.y, glm::linearRand(0.0f, 1.0f));
				(*vel).z = glm::mix(initialForceMin.z, initialForceMax.z, glm::linearRand(0.0f, 1.0f));

				*life = glm::linearRand(lifeRange.x, lifeRange.y);
				*mass = glm::linearRand(0.05f, 1.0f);
				*accel = *vel / *mass;
			}
			*pos += *vel * dt + *accel * 0.5f * (dt*dt);
			*vel += dt;
			*life -= dt;
			
		}
	}
}

void ParticleEmmiter::draw(Camera _camera)
{
	Attribute* attrib = vao.getAttribute(AttributeLocations::VERTEX);

	glBindVertexArray(vao.getVAO());
	glEnableVertexAttribArray(attrib->getAttribLocation());
	glBindBuffer(GL_ARRAY_BUFFER, vao.getVBO(VERTEX));
	attrib->data = particles.position;
	glBufferSubData(GL_ARRAY_BUFFER, 0, attrib->getNumElements() * attrib->getDataSize(), attrib->data);

	if (texture)
	{
		texture->bind(GL_TEXTURE0, GL_TEXTURE1);
	}

	material->shader->bind();
	material->mat4Uniforms["u_mvp"] = _camera.getViewProj();
	material->mat4Uniforms["u_mv"] = _camera.getView();
	material->mat4Uniforms["u_proj"] = _camera.getProj();
	material->intUniforms["u_tex"] = 0;
	material->sendUniforms();



	glDepthMask(GL_FALSE);
	vao.draw();
	glDepthMask(GL_TRUE);

}

void ParticleEmmiter::freeMemory()
{
	if (allocated)
	{
		delete[] particles.position;
		delete[] particles.velocities;
		delete[] particles.lives;
		delete[] particles.acceleration;
		delete[] particles.masses;

		particles.position = nullptr;
		particles.velocities = nullptr;
		particles.lives = nullptr;
		particles.acceleration = nullptr;
		particles.masses = nullptr;

		allocated = false;
		numParticles = 0;
	}
}