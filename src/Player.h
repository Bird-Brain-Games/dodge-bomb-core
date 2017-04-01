#pragma once

#include "GameObject.h"
#include "Bomb.h"
#include "controller.h"
#include "particles.h"
#include "sound engine.h"
#include <map>

class readyUpRing : public GameObject
{
public:
	readyUpRing(glm::vec3 position,
		std::shared_ptr<Loader> _mesh,
		std::shared_ptr<Material> _material,
		std::shared_ptr<Texture> _texture,
		int _playerNum);

	inline int getPlayerNum() { return playerNum; }
	void setPosition(glm::vec3);
	void update(float dt);
	void setReady(bool ready);

private:
	int playerNum;
	bool isReady;
};

// Player class takes in player input and performs movement
// It also handles collision between players and bombs.
class Player : public GameObject
{
public:
	enum PLAYER_STATE
	{
		P_INACTIVE,
		P_NORMAL,
		P_INVINCIBLE,
		P_DEAD
	};

public:
	Player(glm::vec3 position,
		std::shared_ptr<Loader> _mesh,
		std::shared_ptr<Material> _material,
		std::shared_ptr<Texture> _texture, 
		int _playerNum,
		std::map<std::string, Sound>* soundTemplates);
	Player(Player&);
	~Player();

	void initParticles(std::shared_ptr<Material> _material, std::shared_ptr<Texture> _texture);


	void handleInput(float dt);

	void update(float deltaT, bool canMove);
	void draw(Camera &camera, Camera& shadow);
	void drawParticles(Camera& camera);

	void attachRigidBody(std::unique_ptr<RigidBody> &_rb);
	void attachBombManager(std::shared_ptr<BombManager> _manager);
	void takeDamage(int damage);
	void setHealth(int newHealth) { health = newHealth; }
	void reset(glm::vec3 newPos);	// Reset values and positions

	void checkCollisionWith(GameObject* other);
	void checkCollisionWith(Bomb* other, bool inReadyUp);
	void checkCollisionWith(Explosion* other, bool inReadyUp);
	void checkCollisionWith(readyUpRing* other);
	int getPlayerNum();
	glm::vec3 getCurrentVelocity();

	inline bool isReady() { return ready; }
	inline bool isActive() { return currentState != P_INACTIVE; }
	inline void setActive(bool active) { currentState = (active) ? P_NORMAL : P_INACTIVE; }

	void setAnim(std::string);
	int getHealth();

	Controller* getController();

	ParticleEmmiter smoke;// used to show when the player is damaged
	ParticleEmmiter sparks;
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

	// Sounds
	Sound s_damage;
	Sound s_footstep;
	bool moving;

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