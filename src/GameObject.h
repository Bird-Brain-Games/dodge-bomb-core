// GameObject.h
// - Class that represents a object in game
// - Bare minimum must have
// -- a renderable (a sprite, a 3d obj, a wavefront obj)
// -- A position in gamespace (x,y,z)
// - additional properties may be
// -- velocity
// -- acceleration
// -- rotations (x, y, z)
// -- texture
// -- collision bounds (radius, xMins and xMaxs for cube)
#pragma once
#include <memory>
#include <string>
#include <GLM\glm.hpp>
#include <btBulletDynamicsCommon.h>
#include <iostream>
#include "loadObject.h"
#include "Shader.h"
#include "ANILoader.h"
#include "camera.h"
#include "material.h"
#include "controller.h"
class RigidBody;


class GameObject
{
public:
	GameObject(Loader* _model, RigidBody* _body, Texture* _tex, std::shared_ptr<Material> _material, std::string _tag = "Undefined");
	~GameObject();

	virtual void draw(Camera);
	virtual void update(float deltaT);

	void setTransform(glm::vec3 pos, glm::vec4 orientation = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	RigidBody* getRigidBody() { return body; };


private:
	//the Controllers 
	//Controller con;
	// Physics rigid body with Bullet
	RigidBody* body;

	// Used for non rigid body things.
	glm::mat4x4 worldTransform;
	//


	// loaded obj file
	std::shared_ptr<Loader> model;

	std::shared_ptr<Material> material;

	// Texture
	Texture* tex;

	// Identifier Tag???
	std::string const tag;

	// Unique name
	std::string name;
};