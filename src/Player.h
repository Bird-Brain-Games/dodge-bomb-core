#pragma once

#include "GameObject.h"
#include "Bomb.h"
#include "controller.h"

// Player class takes in player input and performs movement
// It also handles collision between players and bombs.
class Player : public GameObject
{
public:
	Player(glm::vec3 position,
		std::shared_ptr<Loader> _mesh,
		std::shared_ptr<Material> _material,
		std::shared_ptr<Texture> _texture, 
		int playerNum);
	Player(Player&);
	~Player();

	void handleInput();

	void update(float deltaT);
	void draw(Camera _camera);
	void attachRigidBody(std::unique_ptr<RigidBody> &_rb);

	void checkCollisionWith(GameObject* other);

private:
	Controller con;
	float angle;
	std::unique_ptr<Bomb> bomb;
};