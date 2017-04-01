
#include "Player.h"
#include <iostream>
#include "SoundDriver.h"

const float degToRad = 3.14159f / 180.0f;

float Player::maxInvincibleTime = 3.0f;
float Player::pauseTime = 0.5f;
int Player::maxHealth = 2;
float Player::maxBombCooldown = 1.0f;
float Player::flashInterval = 0.2f;
float Player::playerSpeed = 50.0f;

// Dash values
float Player::dashImpulse = 30.0f;
float Player::dashMinSpeed = Player::playerSpeed - 20.0f;	// when the player reaches this, stop dashing
float Player::maxDashCooldown = 2.0f;
float Player::maxDashDuration = 0.35f;	// max length of dash in seconds

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
	throwingForce(25.0f)
{
	reset(position);
	playerNum = _playerNum;
	setScale(glm::vec3(0.75f));
	setActive(false);
}

Player::Player(Player& other)
	: GameObject(other),
	con(other.con.getPlayerNum()+1),
	bombCooldown(maxBombCooldown),
	currentAngle(0.0f),
	throwingForce(other.throwingForce)
{
	reset(other.getWorldPosition());
	setActive(false);
}

Player::~Player()
{

}


void Player::initParticles(std::shared_ptr<Material> _material, std::shared_ptr<Texture> _texture)
{
	//make smoker slower. somewhere between 100-250. die faster so not as tall.
	smoke.lifeRange = glm::vec3(0.5, 0.85, 0.0); // this should be a vec2, rands life from x to y.
	smoke.initialForceMin = glm::vec3(-1.5, 3.0, -1.5);
	smoke.initialForceMax = glm::vec3( 1.5, 5.0,  1.5);
	smoke.initialPosition = glm::vec3(0.0f, 5.0f, 0.0f);

	smoke.size = 2.5f;
	smoke.initialGravity = glm::vec3(0.0f, 2.3f, 0.0f);
	smoke.colour = glm::vec3(GameObject::outlineColour);

	smoke.material = _material;
	smoke.texture = _texture;
	smoke.initialize(250);
	smoke.play();
	smoke.dimensions = glm::vec2(5.0f, 2.0f);
	smoke.max = 5;
}

void Player::draw(Camera &camera, Camera& shadow)
{
	if (currentState == P_NORMAL)
	{
		GameObject::draw(camera, shadow);
	}
	else if (currentState == P_INVINCIBLE)
	{
		// If they're still in the pause time, draw them,
		// And if they're invincible make them flash
		if (!isFlashing)
		{
			GameObject::draw(camera, shadow);
		}
	}
}

void Player::drawParticles(Camera& camera)
{
	smoke.draw(camera);
}

int Player::getHealth()
{
	return health;
}

Controller* Player::getController()
{
	return &con;
}

void Player::update(float dt, bool canMove)
{
	// Update the bomb cooldown
	smoke.initialPosition = getWorldPosition() + glm::vec3(0.0, 2.2, 0.0);

	glm::vec3 velocity = rigidBody->getLinearVelocity() * glm::vec3(0.3);
	smoke.update(dt, velocity);
	if (currentCooldown > 0.0f)
	{
		currentCooldown -= dt;
		if (currentCooldown < 0.0f)
			currentCooldown = 0.0f;
	}

	// Update the dash cooldown
	if (currentDashCooldown > 0.0f)
	{
		currentDashCooldown -= dt;
		if (currentDashCooldown < 0.0f)
			currentDashCooldown = 0.0f;
	}

	// Check dashing status
	if (isDashing)
	{
		// Update the dash duration
		if (currentDashDuration > 0.0f)
		{
			currentDashDuration -= dt;
			if (currentDashDuration < 0.0f)
				currentDashDuration = 0.0f;
		}

		glm::vec3 currentVelocity = rigidBody->getLinearVelocity();
		if (currentDashDuration <= 0.0f) //|| glm::length(currentVelocity) <= dashMinSpeed)
		{
			isDashing = false;
			rigidBody->setLinearVelocity(glm::vec3(0.0f));
			//rigidBody->setLinearVelocity(
				//glm::normalize(glm::vec3(-glm::sin(currentAngle), 0.0f, 
					//glm::cos(currentAngle))) * playerSpeed);
		}
	}

	switch (currentState)
	{
	case P_NORMAL:
		if (canMove) handleInput(dt);
		rigidBody->getBody()->setLinearFactor(btVector3(1.0f, 0.5f, 1.0f));
		break;

	case P_INVINCIBLE:
		invincibleTime -= dt;
		// If they're not in the pause time, let them move
		if (maxInvincibleTime - invincibleTime > pauseTime)
		{
			if (canMove) handleInput(dt);

			con.setVibration(0, 0);

			// Handle player flashing when invincible
			flashTime += dt;
			if (flashTime >= flashInterval)
			{
				isFlashing = !isFlashing;
				flashTime -= flashInterval;
			}
		}

		if (invincibleTime <= 0.0f)
		{
			invincibleTime = 0.0f;
			currentState = P_NORMAL;
		}
		break;

	case P_DEAD:
		rigidBody->getBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
		con.setVibration(0, 0);
		break;

	default:
		break;
	}

	// Allow player to reset all stats (DEBUG)
	if (con.conButton(XINPUT_GAMEPAD_BACK))
		reset(glm::vec3(0.0f, 40.0f, 0.0f));

	GameObject::update(dt);
	mesh->update(dt, bottomAngle, currentAngle);

	// Make it so they can't rotate through physics
	rigidBody->getBody()->setAngularFactor(btVector3(0.0, 0.0, 0.0));	
	rigidBody->getBody()->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));

	// Set the ready status to false if not colliding with ring
	ready = false;
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

	if (!isDashing)
	{
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
			currentAngle = atan2(-LStick.y, LStick.x) + 270 * degToRad;
			this->setRotationAngleY(currentAngle);
		}

		if (hasMoved)
		{
			rigidBody->setLinearVelocity(trans * playerSpeed);
		}
	}

	// Throw a bomb
	if ((con.conButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) || con.conRightTrigger())	&& 
		currentCooldown == 0.0f)
	{
		// Get the direction the player is facing
		glm::vec2 normalized = glm::vec2(0);
		if (con.rightStickMoved())
			normalized = glm::normalize(glm::vec2(RStick.x, RStick.y));
		else
			normalized = glm::vec2(-glm::sin(currentAngle), -glm::cos(currentAngle));
		
		glm::vec2 playerVelocity;
		if (hasMoved || isDashing)
			playerVelocity = glm::vec2(rigidBody->getLinearVelocity().x,
				-rigidBody->getLinearVelocity().z);
		else
			playerVelocity = glm::vec2(0.0f);

		bombManager->throwBomb(this, normalized, playerVelocity, throwingForce);
		mesh->setAnim("throw");
		currentCooldown += bombCooldown;
	}

	// Dash
	if ((con.conButton(XINPUT_GAMEPAD_LEFT_SHOULDER) || con.conLeftTrigger()) && !isDashing && currentDashCooldown <= 0.0f)
	{
		isDashing = true;
		currentDashCooldown = maxDashCooldown;
		currentDashDuration = maxDashDuration;

		// Determine what direction to dash
		glm::vec3 playerDirection;

		if (hasMoved)
		{
			playerDirection = glm::normalize(glm::vec3(rigidBody->getLinearVelocity().x, 0.0f,
				rigidBody->getLinearVelocity().z));
		}
		else
		{
			playerDirection = glm::normalize(
				glm::vec3(-glm::sin(currentAngle), 0.0f, glm::cos(currentAngle)));
		}
		// tell them to dash as if they're at max speed
		glm::vec3 currentVelocity = playerDirection * playerSpeed;
		rigidBody->setLinearVelocity(currentVelocity);
		
		// Add the impulse
		rigidBody->applyCentralImpulse(playerDirection * dashImpulse);
	}
		

	//tells the mesh(skeleton) to update
	bottomAngle = atan2(-LStick.x, LStick.y);
}

void Player::checkCollisionWith(GameObject* other)
{
	std::cout << "GameObject collided with player" << std::endl;
}

void Player::checkCollisionWith(Explosion* other, bool inReadyUp)
{
	if (other->getBombParent() == nullptr) return;
	Bomb* bombParent = other->getBombParent();

	if (currentState == P_NORMAL && 
		bombParent->getPlayerNum() != playerNum)
	{
		takeDamage(1);
		con.setVibration(32000, 16000);

		// If in ready up, don't take damage
		if (inReadyUp) health++;
		//lookDirectlyAtExplosion(other->getWorldPosition() - getWorldPosition());
	}
}

void Player::checkCollisionWith(Bomb* other, bool inReadyUp)
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
			con.setVibration(32000, 16000);

			// If in ready up, don't take damage
			if (inReadyUp) health++;

			//lookDirectlyAtExplosion(other->getWorldPosition() - getWorldPosition());
		}
	}
}

void Player::checkCollisionWith(readyUpRing* other)
{
	if (other == nullptr) return;

	if (other->getPlayerNum() == playerNum)
	{
		ready = true;
		other->setReady(ready);
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
	flashTime = 0.0f;
	isFlashing = false;
	currentState = P_INVINCIBLE;
	std::cout << "Player " << playerNum << " took damage" << std::endl;
	rigidBody->setLinearVelocity(glm::vec3(0.0f));
	isDashing = false;

	health--;
	if (health <= 0)
	{
		std::cout << "Player " << playerNum << " is dead" << std::endl;
		currentState = P_DEAD;
		setPosition(glm::vec3(0.0f, -15.0f, 0.0f));
	}
	else if (health == 1)
	{
		smoke.play();
	}
}

void Player::reset(glm::vec3 newPos)
{
	setPosition(newPos);
	health = maxHealth;
	currentCooldown = 0.0f;
	currentState = P_NORMAL;
	con.setVibration(0, 0);
	isDashing = false;
	ready = false;
	currentDashCooldown = 0.0f;
	currentDashDuration = 0.0f;
}

glm::vec3 Player::getCurrentVelocity()
{
	return rigidBody->getLinearVelocity();
}

//void Player::lookDirectlyAtExplosion(glm::vec3 direction)
//{
//	direction = glm::normalize(direction);
//	float angleY = 
//}


///////////////////////////////////////////////////////////////////////////////
////////////////////////////////	Ring class

readyUpRing::readyUpRing(glm::vec3 position,
	std::shared_ptr<Loader> _mesh,
	std::shared_ptr<Material> _material,
	std::shared_ptr<Texture> _texture,
	int _playerNum)
	: GameObject(position, _mesh, _material, _texture),
	playerNum(_playerNum)
{
	colliderType = COLLIDER_TYPE::READYUP;
	isReady = false;
}

void readyUpRing::setPosition(glm::vec3 newPos)
{
	GameObject::setPosition(newPos);
}

void readyUpRing::update(float dt)
{
	GameObject::update(dt);
}

void readyUpRing::setReady(bool ready)
{
	// If there is a change, change the emissive value
	isReady = ready;
	emissiveLight = (isReady) ? 0.5f : 0.0f;
}

