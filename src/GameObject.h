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

#include <string>
#include <GLM\glm.hpp>
#include "loadObject.h"
#include "Shader.h"
#include <iostream>
#include <btBulletDynamicsCommon.h>
#include "ANILoader.h"
class RigidBody;


class GameObject
{
public:
	GameObject(LoadObject* _model, RigidBody* _body);
	GameObject(LoadObject* _model, RigidBody* _body, Texture* _tex, std::string _tag = "Undefined");
	GameObject(Holder* _hierarchy, RigidBody* _body, Texture* _tex);
	~GameObject();

	virtual void draw(Shader *s);
	virtual void update(float deltaT);

	void setTransform(glm::vec3 pos, glm::vec4 orientation);

	RigidBody* getRigidBody() { return body; };

private:
	// Physics rigid body with Bullet
	RigidBody* body;

	glm::mat4x4 worldTransform;

	// loaded obj file
	LoadObject* model;
	//holds the VBO and hierarchy data.
	Holder* hierarchy;

	// Texture
	Texture* tex;

	// Identifier Tag
	std::string const tag;

	// Unique name
	std::string name;
};