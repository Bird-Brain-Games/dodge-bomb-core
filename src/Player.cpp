#include "Player.h"
#include <iostream>

const float degToRad = 3.14159f / 180.0f;

float Player::maxInvincibleTime = 3.0f;
float Player::pauseTime = 0.5f;
int Player::maxHealth = 2;
float Player::maxBombCooldown = 1.0f;

Player::Player(glm::vec3 position,
	std::shared_ptr<Loader> _mesh,
	std::shared_ptr<Material> _material,
	std::shared_ptr<Texture> _texture,
	int _playerNum)
	:
	GameObject(position, _mesh, _material, _texture), 
	con(_playerNum),
	bombCooldown(maxBombCooldown),
	currentAngle(0.0f),
	throwingForce(30.0f)
{
	reset(position);
	playerNum = _playerNum;
}

Player::Player(Player& other)
	: GameObject(other),
	con(other.con.getPlayerNum()+1),
	bombCooldown(maxBombCooldown),
	currentAngle(0.0f),
	throwingForce(other.throwingForce)
{
	reset(other.getWorldPosition());
}

Player::~Player()
{

}

void Player::draw(Camera &camera)
{
	if (currentState == P_NORMAL)
	{
		GameObject::draw(camera);
	}
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

	switch (currentState)
	{
	case P_NORMAL:
		handleInput(dt);
		//rigidBody->getBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
		break;

	case P_INVINCIBLE:
		invincibleTime -= dt;
		// If they're still in the pause time, don't let them move
		if (maxInvincibleTime - invincibleTime > pauseTime)
			handleInput(dt);

		if (invincibleTime <= 0.0f)
		{
			invincibleTime = 0.0f;
			currentState = P_NORMAL;
		}
		break;

	case P_DEAD:
		rigidBody->getBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
		break;

	default:
		break;
	}

	// Allow player to reset all stats (DEBUG)
	if (con.conButton(XINPUT_GAMEPAD_LEFT_SHOULDER))
		reset(glm::vec3(0.0f, 40.0f, 0.0f));

	GameObject::update(dt);
	mesh->update(dt, bottomAngle, currentAngle);

	// Make it so they can't rotate along other than the Y axis.
	rigidBody->getBody()->setAngularFactor(btVector3(0.0, 1.0, 0.0));	
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
		this->setRotationAngleY(currentAngle);
	}

	else if (con.leftStickMoved() && !con.rightStickMoved())
	{
		//currentAngle = atan2(LStick.y, LStick.x) + 180 * degToRad;
		//this->setRotationAngleY(angle);
	}
	
	if (hasMoved)
	{
		setPosition(getWorldPosition() + trans);
		//rigidBody->getBody()->
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
	bottomAngle = atan2(-LStick.x, LStick.y);
}

void Player::checkCollisionWith(GameObject* other)
{
	std::cout << "GameObject collided with player" << std::endl;
}

void Player::checkCollisionWith(Explosion* other)
{
	if (other->getBombParent() == nullptr) return;
	Bomb* bombParent = other->getBombParent();

	if (bombParent->getPlayerNum() == playerNum)
	{

	}
	else if (currentState == P_NORMAL)
	{
		takeDamage(1);
	}
}

void Player::checkCollisionWith(Bomb* other)
{
	if (other == nullptr) return;
	// If the player is colliding with their own bomb
	if (other->getPlayerNum() == playerNum)
	{

	}
	else if (currentState == P_NORMAL)
	{
		if (other->getCurrentState() != BOMB_STATE::DONE)
		{
			other->explode();
			takeDamage(1);
		}
	}	
}

void Player::attachRigidBody(std::unique_ptr<RigidBody> &_rb)
{
	GameObject::attachRigidBody(_rb);
	rigidBody->setDeactivationMode(DISABLE_DEACTIVATION);

	// Essentially friction
	rigidBody->getBody()->setDamping(0.2, 1.0);
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

void Player::takeDamage(int damage)
{
	invincibleTime = maxInvincibleTime;
	currentState = P_INVINCIBLE;
	std::cout << "Player " << playerNum << " took damage" << std::endl;

	health--;
	if (health <= 0)
	{
		std::cout << "Player " << playerNum << " is dead" << std::endl;
		currentState = P_DEAD;
		setPosition(glm::vec3(0.0f, -15.0f, 0.0f));
	}
}

void Player::reset(glm::vec3 newPos)
{
	setPosition(newPos);
	health = maxHealth;
	currentCooldown = 0.0f;
	currentState = P_NORMAL;
}