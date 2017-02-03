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
#include "loadObject.h"
#include "Shader.h"
#include <iostream>
#include <btBulletDynamicsCommon.h>
#include "ANILoader.h"
#include "camera.h"
#include "material.h"
class RigidBody;


class GameObject
{
public:
	GameObject(std::shared_ptr<LoadObject> _model, RigidBody* _body, std::shared_ptr<Material> _material);
	GameObject(std::shared_ptr<LoadObject> _model, RigidBody* _body, Texture* _tex, std::shared_ptr<Material> _material, std::string _tag = "Undefined");
	GameObject(std::shared_ptr<Holder> _hierarchy, RigidBody* _body, Texture* _tex, std::shared_ptr<Material> _material);
	~GameObject();

	virtual void draw(Camera);
	virtual void update(float deltaT);

	void setTransform(glm::vec3 pos, glm::vec4 orientation);

	RigidBody* getRigidBody() { return body; };

private:
	// Physics rigid body with Bullet
	RigidBody* body;

	glm::mat4x4 worldTransform;

	// loaded obj file
	std::shared_ptr<LoadObject> model;
	//holds the VBO and hierarchy data.
	std::shared_ptr<Holder> hierarchy;

	std::shared_ptr<Material> material;

	// Texture
	Texture* tex;

	// Identifier Tag???
	std::string const tag;

	// Unique name
	std::string name;
};