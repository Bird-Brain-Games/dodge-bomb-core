#include "Player.h"
#include <iostream>

const float degToRad = 3.14159f / 180.0f;

float Player::maxInvincibleTime = 3.0f;
float Player::pauseTime = 0.5f;
int Player::maxHealth = 2;

Player::Player(glm::vec3 position,
	std::shared_ptr<Holder> _mesh,
	std::shared_ptr<Material> _material,
	std::shared_ptr<Texture> _texture,
	int _playerNum)
	:
	GameObject(position, _mesh, _material, _texture),
	con(_playerNum),
	currentCooldown(0.0f),
	bombCooldown(1.0f),
	currentAngle(0.0f),
	throwingForce(5.0f),
	health(maxHealth),
	currentState(P_NORMAL)
{
	playerNum = _playerNum;

	
	animation = _mesh; 
	topFrame = 0; botFrame = 0; 
	top = animation->getAnimations().at("idle");
	bot = animation->getAnimations().at("idle");
}

Player::Player(Player& other)
	: GameObject(other),
	con(other.con.getPlayerNum() + 1),
	currentCooldown(0.0f),
	bombCooldown(1.0f),
	currentAngle(0.0f),
	health(maxHealth),
	throwingForce(other.throwingForce),
	currentState(P_NORMAL)
{

}

Player::~Player()
{

}

void Player::draw(Camera &camera)
{
	if (currentState == P_NORMAL)
	{
		material->shader->sendUniformMat4("BoneMatrixArray", multipliedMatricies[0], 28);
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

	GameObject::update(dt);
	topFrame++;
	botFrame++;

	if (playerNum == 0)
	{
		topFrame = 5;
		botFrame = 5;
	}

	//std::cout << topFrame << " " << botFrame << std::endl;

	animation->setPlayer(top, bot, &topFrame, &botFrame);
	//std::cout << bot << std::endl;
	multipliedMatricies = mesh->update(dt, bottomAngle, currentAngle);

	if (topFrame >= top->jointAnimation->numFrames - 1)
	{
		topFrame = 0;
	//	std::cout << "idle" << std::endl;
		top = animation->getAnimations().at("idle");
	}
	if (botFrame >= bot->jointAnimation->numFrames - 1)
	{
		botFrame = 0;
	//	std::cout << "idle" << std::endl;
		bot = animation->getAnimations().at("idle");
	}

	rigidBody->getBody()->setAngularFactor(btVector3(0, 1, 0));	// Every frame?
	if (playerNum == 0)
	{
		std::cout << topFrame << " " << botFrame << std::endl;
	}
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
		setAnimations("walk");
		trans.x = LStick.x / 2;
		hasMoved = true;
	}
	if (LStick.y < -0.1 || LStick.y > 0.1)
	{
		setAnimations("walk");
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
		setAnimations("throw");
		currentCooldown += bombCooldown;
	}


	//tells the mesh(skeleton) to update
	bottomAngle = atan2(-LStick.x, LStick.y);
}

void Player::checkCollisionWith(GameObject* other)
{
	std::cout << "GameObject collided with player" << std::endl;
}

void Player::checkCollisionWith(Bomb* other)
{
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
	//std::cout << "C: bomb " << other->getPlayerNum() << " with player " << playerNum << std::endl;

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


void Player::setAnimations(std::string _name)
{

	
	if (animation->getAnimations().count(_name) == 1)
	{
		if (top == animation->getAnimations().at("idle") || top == animation->getAnimations().at("walk"))
		{
			if (top != animation->getAnimations().at(_name))
			{
				std::cout << _name << std::endl;
			top = animation->getAnimations().at(_name);
			topFrame = 0;
			}
		}
		if (bot == animation->getAnimations().at("idle") || bot == animation->getAnimations().at("throw"))
		{
			if (bot != animation->getAnimations().at(_name))
			{
				std::cout << _name << std::endl;
				bot = animation->getAnimations().at(_name);
				botFrame = 0;
			}

		}
	}
	else
		std::cout << "error the animation " + _name + " does not exist" << std::endl;
}