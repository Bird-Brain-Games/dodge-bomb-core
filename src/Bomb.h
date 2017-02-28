#pragma once

#include "GameObject.h"

// Bomb emitter class tracks active bombs
// And assigns bomb properties upon creation

/*
holds 4 bomb objects, 1 for each player
when player asks to throw bomb, sends call to emitter 
with player number, and the implulse, and creates 
the game object using copy constructor
*/

class BombManager
{

};

// Bomb object 
class Bomb : public GameObject
{
public:
	Bomb(glm::vec3 position,
		std::shared_ptr<Loader> _mesh,
		std::shared_ptr<Material> _material,
		std::shared_ptr<Texture> _texture,
		int playerNum);
	~Bomb();


private:
	float duration;
	int playerNum;

};