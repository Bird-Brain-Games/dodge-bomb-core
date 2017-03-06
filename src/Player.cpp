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
	bombCooldown(1.0f),
	currentAngle(0.0f),
	throwingForce(5.0f)
{
	playerNum = _playerNum;
}

Player::Player(Player& other)
	: GameObject(other),
	con(other.con.getPlayerNum()+1),
	currentCooldown(0.0f),
	bombCooldown(1.0f),
	currentAngle(0.0f),
	throwingForce(other.throwingForce)
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

	// the mesh updates is now down in this call because we need to pass certain info about the controls to the htr 
	handleInput(dt);



	// orignal function order
	GameObject::update(dt);

	//was crashing have no idea why. needs fixing
//	rigidBody->getBody()->setAngularFactor(btVector3(0, 1, 0));	// Every frame?
}

void Player::handleInput(float dt)
{
	if (!con.connected())
	{
		//std::cerr << "Error: Controller disconnected" << std::endl;
		return;
	}


	bool hasMoved = false;

	// Update the direction of the player
	// Based on the position of the right stick
	Coords RStick = con.getRightStick();
	float angle = atan2(-RStick.y, RStick.x) + 270 * degToRad;


	// Check if the player has moved the left stick
	Coords LStick = con.getLeftStick();
	glm::vec3 trans = glm::vec3(0.0f);

	if (LStick.x < -0.1 || LStick.x > 0.1)
	{
		mesh->setAnim("walk");
		trans.x = LStick.x / 2;
		hasMoved = true;
	}
	if (LStick.y < -0.1 || LStick.y > 0.1)
	{
		mesh->setAnim("walk");
		trans.z = -LStick.y / 2;
		hasMoved = true;
	}

	
	if (currentAngle != angle && con.rightStickMoved())
	{
		currentAngle = angle;
	}

	else if (con.leftStickMoved() && !con.rightStickMoved())
	{
		//currentAngle = atan2(LStick.y, LStick.x) + 180 * degToRad;
		//this->setRotationAngleY(angle);
	}
	this->setRotationAngleY(currentAngle);
	if (hasMoved)
	{
		setPosition(getWorldPosition() + trans);
		//player->rigidBody->getBody()->applyCentralImpulse(btVector3(-stick.y, 0, -stick.x));
	}


	// Throw a bomb (crashes my code so im using left shoulder for animation throw)
	if (con.conButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) && currentCooldown == 0.0f)
	{
		glm::vec2 normalized = glm::vec2(0);
		if (con.rightStickMoved())
			normalized = glm::normalize(glm::vec2(RStick.x, RStick.y));

		
		bombManager->throwBomb(this, normalized, throwingForce);
		mesh->setAnim("throw");
		currentCooldown += bombCooldown;
	}
		

	//tells the mesh(skeleton) to update
	angle = atan2(-LStick.x, LStick.y);
	mesh->update(dt, angle, currentAngle);
}

void Player::checkCollisionWith(GameObject* other)
{
	std::cout << "GameObject collided with player" << std::endl;
}

void Player::checkCollisionWith(Bomb* other)
{
	if (other->getPlayerNum() == playerNum)
	{
		//std::cout << "Bomb collided with the player who threw it" << std::endl;
	}
	std::cout << "bomb collided with player " << playerNum << std::endl;
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

void Player::setAnim(std::string _name)
{
	mesh->setAnim(_name);
}