#include "Bomb.h"
#include "Player.h"

///////////////////////////////////////////////////////////////////////////////
////////////////////////	BOMB MANAGER	///////////////////////////////////
BombManager::BombManager()
{
	impulseY = 75.0f;
}

bool BombManager::init(std::shared_ptr<Loader> _mesh,
	std::shared_ptr<Texture> _p1,
	std::shared_ptr<Texture> _p2,
	std::shared_ptr<Texture> _p3,
	std::shared_ptr<Texture> _p4,
	std::shared_ptr<Material> _material,
	std::string bodyPath)
{
	if (initialized) return false;

	mesh = _mesh;
	material = _material;
	textures.push_back(_p1);
	textures.push_back(_p2);
	textures.push_back(_p3);
	textures.push_back(_p4);

	// Load the rigidBodies


	// Create the bomb object templates
	for (int i = 0; i < 4; i++)
	{
		std::shared_ptr<Bomb> bombObject = std::make_shared<Bomb>(
			glm::vec3(-100.0f),
			mesh,
			material,
			textures.at(i),
			i);

		// Create the rigidbody
		std::unique_ptr<RigidBody> rb = std::make_unique<RigidBody>(
			btBroadphaseProxy::SensorTrigger);
		rb->load(bodyPath);
		bombObject->attachRigidBody(rb);


		bombTemplates.push_back(bombObject);
	}




	initialized = true;
	return true;
}

void BombManager::update(float dt)
{
	for (auto it : activeBombs)
	{
		it->update(dt);
	}
}

void BombManager::draw(Camera& camera)
{
	for (auto it : activeBombs)
	{
		it->draw(camera);
	}
}

void BombManager::throwBomb(std::shared_ptr<Player> player, glm::vec2 direction, float force)
{
	int playerNum = player->getPlayerNum();
	std::shared_ptr<Bomb> newBomb = std::make_shared<Bomb>(*bombTemplates.at(playerNum));
	newBomb->attachPlayerPtr(player);
	activeBombs.push_back(newBomb);
	newBomb->throwBomb(glm::vec3(direction.x, impulseY, direction.y), force);
	
}



///////////////////////////////////////////////////////////////////////////////
////////////////////////////	BOMB	///////////////////////////////////////
Bomb::Bomb(glm::vec3 position,
	std::shared_ptr<Loader> _mesh,
	std::shared_ptr<Material> _material,
	std::shared_ptr<Texture> _texture,
	int _playerNum)	
	: GameObject(position, _mesh, _material, _texture),
	playerNum(_playerNum)
{

}

Bomb::~Bomb()
{

}

Bomb::Bomb(Bomb& other)
	: GameObject(other),
	playerNum(other.playerNum)
{

}

void Bomb::attachPlayerPtr(std::shared_ptr<Player> _playerPtr)
{
	playerPtr = _playerPtr;
}

void Bomb::throwBomb(glm::vec3 direction, float force)
{
	setPosition(playerPtr->getWorldPosition() + glm::vec3(0.0f, 1.0f, 0.0f));

	//	bomb->getRigidBody()->getBody()->setLinearVelocity(btVector3(0, 0, 0));
	//	bomb->getRigidBody()->getBody()->clearForces();//clears force not impulse?

	rigidBody->getBody()->applyCentralImpulse(
		btVector3(direction.x * force, direction.y, direction.z * force));
}

void Bomb::draw(Camera& camera)
{
	GameObject::draw(camera);
}

void Bomb::update(float dt)
{
	GameObject::update(dt);
}