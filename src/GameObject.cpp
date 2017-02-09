#include "GameObject.h"

#include <iostream>
#include <fstream>

#include "RigidBody.h"



GameObject::GameObject(Loader* _model, RigidBody* _body, Texture* _tex, std::shared_ptr<Material> _material, std::string _tag)
	: tag(_tag)
{
	model.reset(_model);
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
	model->draw(material->shader);
	material->shader->unbind();
}

void GameObject::update(float dt)
{
	model->update(dt);

}

void GameObject::setTransform(glm::vec3 _pos, glm::vec4 _orientation)
{
	getRigidBody()->setWorldTransform(_pos, _orientation);

}

void GameObject::setTransform(glm::mat4x4 transform)
{
	getRigidBody()->setWorldTransform(transform);
}