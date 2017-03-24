#pragma once

#include "GameObject.h"
#include "Bomb.h"
#include "controller.h"

class readyUpRing : public GameObject
{
public:
	readyUpRing(glm::vec3 position,
		std::shared_ptr<Loader> _mesh,
		std::shared_ptr<Material> _material,
		std::shared_ptr<Texture> _texture,
		int _playerNum);

	inline int getPlayerNum() { return playerNum; }

private:
	int playerNum;
};

enum PLAYER_STATE
{
	P_NORMAL,
	P_INVINCIBLE,
	P_DEAD
};

// Player class takes in player input and performs movement
// It also handles collision between players and bombs.
class Player : public GameObject
{
public:
	Player(glm::vec3 position,
		std::shared_ptr<Loader> _mesh,
		std::shared_ptr<Material> _material,
		std::shared_ptr<Texture> _texture, 
		int _playerNum);
	Player(Player&);
	~Player();

	void handleInput(float dt);

	void update(float deltaT);
	void draw(Camera &camera);
	void attachRigidBody(std::unique_ptr<RigidBody> &_rb);
	void attachBombManager(std::shared_ptr<BombManager> _manager);
	void takeDamage(int damage);
	void reset(glm::vec3 newPos);	// Reset values and positions

	void checkCollisionWith(GameObject* other);
	void checkCollisionWith(Bomb* other);
	void checkCollisionWith(Explosion* other);
	void checkCollisionWith(readyUpRing* other);
	int getPlayerNum();
	glm::vec3 getCurrentVelocity();

	inline bool isReady() { return ready; }

	void setAnim(std::string);
	int getHealth();

	Controller* getController();

private:
	void lookDirectlyAtExplosion(glm::vec3 direction);

private:

	Controller con;
	int playerNum;

	float bombCooldown;		// The value the cooldown gets set to when thrown
	float currentCooldown;	// The current cooldown on the bomb throw
	std::shared_ptr<BombManager> bombManager;
	float currentAngle;
	float bottomAngle; //used for animation rotation.
	glm::vec3 throwingForce;

	// Player stats
	int health;
	PLAYER_STATE currentState;
	bool ready;	// For ready up

	// Invincibility variables
	bool isFlashing;
	float invincibleTime;
	float flashTime;

	// Dash variables
	bool isDashing;
	float currentDashCooldown;
	float currentDashDuration;

private:
	// Player stats
	static int maxHealth;
	static float maxBombCooldown;

	// Invincibility values
	static float maxInvincibleTime;
	static float pauseTime;
	static float flashInterval;
	static float playerSpeed;

	// Dash values
	static float dashImpulse;
	static float dashMinSpeed;	// when the player reaches this, stop dashing
	static float maxDashCooldown;
	static float maxDashDuration;
};