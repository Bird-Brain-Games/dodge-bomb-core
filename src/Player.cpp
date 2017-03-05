#include "Player.h"
#include <iostream>

const float degToRad = 3.14159f / 180.0f;

Player::Player(glm::vec3 position,
	std::shared_ptr<Loader> _mesh,
	std::shared_ptr<Material> _material,
	std::shared_ptr<Texture> _texture,
	int _playerNum)
	:
	GameObject(position, _mesh, _material, _texture), 
	con(_playerNum),
	currentCooldown(0.0f),
	bombCooldown(1.0f)
{
	playerNum = _playerNum;
}

Player::Player(Player& other)
	: GameObject(other),
	con(other.con.getPlayerNum()+1),
	currentCooldown(0.0f),
	bombCooldown(1.0f)
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

void Player::update(float dt)
{
	// Update the bomb cooldown
	if (currentCooldown > 0.0f)
	{
		currentCooldown -= dt;
		if (currentCooldown < 0.0f)
			currentCooldown = 0.0f;
	}

	handleInput();
	GameObject::update(dt);
	rigidBody->getBody()->setAngularFactor(btVector3(0, 1, 0));	// Every frame?
}

void Player::handleInput()
{
	if (!con.connected())
	{
		//std::cerr << "Error: Controller disconnected" << std::endl;
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
	float angle = atan2(-RStick.y, RStick.x) + 270 * degToRad;
	this->setRotationAngleY(angle);

	if (hasMoved)
	{
		setPosition(getWorldPosition() + trans);
		//player->rigidBody->getBody()->applyCentralImpulse(btVector3(-stick.y, 0, -stick.x));

	}

	// Throw a bomb
	if (con.conButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) && currentCooldown == 0.0f)
	{
		glm::vec2 normalized = glm::vec2(0);
		if (RStick.y > 0.1 || RStick.y < -0.1 || RStick.x > 0.1 || RStick.x < -0.1)
			normalized = glm::normalize(glm::vec2(RStick.x, RStick.y));

		glm::vec3 force(50.0f);
		bombManager->throwBomb(this, normalized, force);

		currentCooldown += bombCooldown;
	}

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

int Player::getPlayerNum()
{
	return playerNum;
}

void Player::attachBombManager(std::shared_ptr<BombManager> manager)
{
	bombManager = manager;
}