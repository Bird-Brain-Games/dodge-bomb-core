#include "Player.h"
#include <iostream>

const float degToRad = 3.14159f / 180.0f;

Player::Player(glm::vec3 position,
	std::shared_ptr<Loader> _mesh,
	std::shared_ptr<Material> _material,
	std::shared_ptr<Texture> _texture,
	int playerNum)
	:
	GameObject(position, _mesh, _material, _texture), 
	con(playerNum)
{
	//bomb = _bomb;
	//bomb->setTransform(glm::vec3(0.0, -15.0, 0.0));

	//timer = 0;
	//duration = 3;
	//thrown = false;
}

Player::Player(Player& other)
	: GameObject(other),
	con(other.con.getPlayerNum()+1)
{

}

Player::~Player()
{

}

void Player::draw(Camera _camera)
{
	GameObject::draw(_camera);
	//bomb->draw(_camera);
}

void Player::update(float _dt)
{

	/*if (thrown == true)
	{
		timer += _dt;
		if (timer > duration)
		{
			timer = 0;
			thrown = false;
			bomb->setTransform(glm::vec3(0.0f, -50.0f, 0.0f));
		}
	}

	*/
	handleInput();
	GameObject::update(_dt);
	rigidBody->getBody()->setAngularFactor(btVector3(0, 1, 0));	// Every frame?
}

void Player::handleInput()
{
	if (!con.connected())
	{
		std::cerr << "Error: Controller disconnected" << std::endl;
		return;
	}

	// Check if the player has moved the left stick
	Coords LStick = con.getLeftStick();
	glm::vec3 trans = glm::vec3(0.0f);

	bool hasMoved = false;
	if (LStick.x < -0.1 || LStick.x > 0.1)
	{
		trans.x = LStick.x / 2;
		hasMoved = true;
	}
	if (LStick.y < -0.1 || LStick.y > 0.1)
	{
		trans.z = -LStick.y / 2;
		hasMoved = true;
	}

	// Update the direction of the player
	// Based on the position of the right stick
	Coords RStick = con.getRightStick();
	angle = atan2(-RStick.y, RStick.x) + 270 * degToRad;
	this->setRotationAngleY(angle);

	if (hasMoved)
	{
		setPosition(getWorldPosition() + trans);
		//player->rigidBody->getBody()->applyCentralImpulse(btVector3(-stick.y, 0, -stick.x));

	}

	//if (con.conButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) && thrown == false)
	//{
	//	bomb->getRigidBody()->getBody()->setLinearVelocity(btVector3(0, 0, 0));
	//	bomb->getRigidBody()->getBody()->clearForces();//clears force not impulse?

	//	glm::vec3 temp = player->getRigidBody()->getWorldTransform()[3];

	//	glm::vec2 normalized = glm::vec2(0);

	//	if (stick.y > 0.1 || stick.y < -0.1 || stick.x > 0.1 || stick.x < -0.1)
	//		normalized = glm::normalize(glm::vec2(stick.x, stick.y));


	//	bomb->setTransform(temp, glm::vec4(0.0f, 0.0, 0.0f, 1.f));

	//	std::cout << "x: " << normalized.x << "y: " << normalized.y << std::endl;

	//	bomb->getRigidBody()->getBody()->applyCentralImpulse(btVector3(normalized.x * 150, 75.0f, normalized.y * 150));

	//	thrown = true;

	//}

}

void Player::checkCollisionWith(GameObject* other)
{
	std::cout << "bomb collided with player" << std::endl;
}

void Player::attachRigidBody(std::unique_ptr<RigidBody> &_rb)
{
	GameObject::attachRigidBody(_rb);
	rigidBody->setDeactivationMode(DISABLE_DEACTIVATION);
}