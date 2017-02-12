#pragma once

#include "GameObject.h"

// Player class takes in player input and performs movement
// It also handles collision between players and bombs.
class Player : public GameObject
{
public:
	Player(Loader* _model, RigidBody* _body, Texture* _tex, std::shared_ptr<Material> _material, std::string _tag = "Undefined");
	~Player();

private:
	
	//btBroadphaseProxy::
};