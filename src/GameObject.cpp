#include "GameObject.h"

#include <iostream>
#include <fstream>

#include "RigidBody.h"

GameObject::GameObject(std::shared_ptr<LoadObject> _model, RigidBody* _body, std::shared_ptr<Material> _material)
	: tag("Undefined")
{
	model = _model;
	body = _body;
	tex = nullptr;
	hierarchy = nullptr;
	material = _material;
}

GameObject::GameObject(std::shared_ptr<LoadObject> _model, RigidBody* _body, Texture* _tex, std::shared_ptr<Material> _material, std::string _tag)
	: tag(_tag)
{
	hierarchy = nullptr;
	model = _model;
	body = _body;
	tex = _tex;
	material = _material;
}

GameObject::GameObject(std::shared_ptr<Holder> _hierarchy, RigidBody* _body, Texture* _tex, std::shared_ptr<Material> _material)
{
	model = nullptr;
	hierarchy = _hierarchy;
	body = _body;
	tex = _tex;
	material = _material;
}

GameObject::~GameObject()
{
	if (body)
	{
		delete body;
	}
	

	model = nullptr;
	body = nullptr;
}

void GameObject::draw(Camera camera)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	material->shader->bind();

	material->shader->uniformMat4x4("mvm", &camera.getView());
	material->shader->uniformMat4x4("prm", &camera.getProj());

	// Bind texture here if has one
	if (tex != nullptr)
	{
		tex->bind(material->shader);
	}

	// Compute local transformation
	material->shader->uniformMat4x4("localTransform", &body->getWorldTransform());
	if (model == nullptr && hierarchy != nullptr)
	{
		hierarchy->draw(material->shader);
	}
	else if (model != nullptr && hierarchy == nullptr)
	{
		model->draw();
	}
	material->shader->unbind();
}

void GameObject::update(float dt)
{
	if (hierarchy)
	{
		hierarchy->update(dt);
	}
}

void GameObject::setTransform(glm::vec3 pos, glm::vec4 orientation)
{
	getRigidBody()->setWorldTransform(pos, orientation);
}