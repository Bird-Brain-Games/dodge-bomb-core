#pragma once

#include "GameObject.h"
#include "Bomb.h"
#include "controller.h"
#include "ANILoader.h"

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
		std::shared_ptr<Holder> _mesh,
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
	void reset();	// Reset values and positions

	void checkCollisionWith(GameObject* other);
	void checkCollisionWith(Bomb* other);
	int getPlayerNum();

	std::vector<glm::mat4> multipliedMatricies;

private:

	Controller con;
	int playerNum;

	float bombCooldown;		// The value the cooldown gets set to when thrown
	float currentCooldown;	// The current cooldown on the bomb throw
	std::shared_ptr<BombManager> bombManager;
	

	glm::vec3 throwingForce;

	//animation stats;
	float bottomAngle; //used for animation rotation.
	float currentAngle;

	void setAnimations(std::string);//used to set our player nodes equal to what animations we want
	std::shared_ptr<Holder> animation; // allows us to call our animation functions without them being part of the loader class.
	int topFrame, botFrame; // for the top and bottom of the skeleton
	Node* top;//points to what our top animations is
	Node* bot;//points to what our bot animations is
	//the matrix we send our shaders.

	// Player stats
	int health;
	float invincibleTime;
	PLAYER_STATE currentState;

	static float maxInvincibleTime;
	static float pauseTime;
	static int maxHealth;

};