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

void GameObject::draw(Camera camera, bool test)
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
	material->shader->uniformMat4x4("localTransform", &worldTransform);
	model->draw(material->shader);
	material->shader->unbind();
}

void GameObject::update(float dt)
{
	model->update(dt);

}

void GameObject::setTransform(glm::vec3 _pos, glm::vec3 _orientation)
{
	if (_orientation == glm::vec3(0.0f))
	{
		getRigidBody()->setWorldTransform(_pos);
		return;
	}
	getRigidBody()->setWorldTransform(_pos, _orientation);

}

void GameObject::setTransform(glm::mat4x4 transform)
{
	getRigidBody()->setWorldTransform(transform);
}



Player::Player(GameObject* _bomb, int _index, Loader* _model, RigidBody* _body, Texture* _tex, std::shared_ptr<Material> _material, std::string _tag)
	:GameObject(_model, _body, _tex, _material, _tag), con(_index)
{
		bomb = _bomb;
		bomb->setTransform(glm::vec3(0.0, -15.0, 0.0));

		timer = 0;
		duration = 3;
		thrown = false;
}

Player::~Player()
{

}

void Player::draw(Camera _camera)
{
	bomb->draw(_camera);
	GameObject::draw(_camera);
	this->getRigidBody()->getBody()->setAngularFactor(btVector3(0, 1, 0));
}

void Player::update(float _dt)
{
	
	if (thrown == true)
	{
		timer += _dt;
		if (timer > duration)
		{
			timer = 0;
			thrown = false;
			bomb->setTransform(glm::vec3(0.0f, -50.0f, 0.0f));
		}
	}

	controls();
	GameObject::update(_dt);
}

const float degToRad = 3.14159f / 180.0f;
void Player::controls()
{
	GameObject* player = this;
	RigidBody * rigid = player->getRigidBody();

	Coords stick = con.getLeftStick();
	glm::vec3 pos = glm::vec3(0.0f);
	//std::cout << stick.x << " y: " << stick.y << std::endl;
	bool motion = false;
	if (stick.x < -0.1 || stick.x > 0.1)
	{
		pos.x = stick.x / 2;
		motion = true;
	}
	if (stick.y < -0.1 || stick.y > 0.1)
	{
		pos.z = -stick.y / 2;
		motion = true;
	}
	stick = con.getRightStick();
	angle = atan2(-stick.y, stick.x) + 270 * degToRad;

	
	glm::vec3 temp = player->getRigidBody()->getWorldTransform()[3];

	player->setTransform(temp, glm::vec4(0.0f, angle, 0.0f, 1.f));

	if (motion == true)
	{

		glm::mat4 transform = rigid->getWorldTransform();
		glm::mat4 translate = glm::translate(pos);
		transform += translate;
		player->setTransform(transform);
		//player->getRigidBody()->getBody()->applyCentralImpulse(btVector3(-stick.y, 0, -stick.x));

	}

	if (con.conButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) && thrown == false)
	{
		bomb->getRigidBody()->getBody()->setLinearVelocity(btVector3(0, 0, 0));
		bomb->getRigidBody()->getBody()->clearForces();//clears force not impulse?

		glm::vec3 temp = player->getRigidBody()->getWorldTransform()[3];

		glm::vec2 normalized = glm::vec2(0);

		if (stick.y > 0.1 || stick.y < -0.1 || stick.x > 0.1 || stick.x < -0.1)
			normalized = glm::normalize(glm::vec2(stick.x, stick.y));


		bomb->setTransform(temp, glm::vec4(0.0f, 0.0, 0.0f, 1.f));

		std::cout << "x: " << normalized.x << "y: " << normalized.y << std::endl;

		bomb->getRigidBody()->getBody()->applyCentralImpulse(btVector3(normalized.x * 150, 75.0f, normalized.y * 150));

		thrown = true;

	}

}

void Player::checkCollisionWith(GameObject* other)
{
	std::cout << "bomb collided with player" << std::endl;
}