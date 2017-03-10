#include "Bomb.h"
#include "Player.h"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
////////////////////////	BOMB MANAGER	///////////////////////////////////
BombManager::BombManager()
{
	impulseY = 75.0f;
	initialized = false;
}

bool BombManager::init(
	std::shared_ptr<Loader> _bombMesh,
	std::shared_ptr<Texture> _p1,
	std::shared_ptr<Texture> _p2,
	std::shared_ptr<Texture> _p3,
	std::shared_ptr<Texture> _p4,
	std::shared_ptr<Loader> _explosionMesh,
	std::shared_ptr<Texture> _explosionTex,
	std::string _explosionBodyPath,
	std::shared_ptr<Material> _material,
	std::string bodyPath)
{
	if (initialized) return false;

	bombMesh = _bombMesh;
	explosionMesh = _explosionMesh;
	material = _material;
	textures.push_back(_p1);
	textures.push_back(_p2);
	textures.push_back(_p3);
	textures.push_back(_p4);
	textures.push_back(_explosionTex);

	// Create the explosion object
	explosion = std::make_shared<Explosion>(
		glm::vec3(-100.0f),
		explosionMesh,
		material,
		textures[4],
		nullptr);

	// Create the explosion rigidBody
	std::unique_ptr<RigidBody> explosionBody = 
		std::make_unique<RigidBody>(btBroadphaseProxy::SensorTrigger);
	explosionBody->load(_explosionBodyPath, btCollisionObject::CF_KINEMATIC_OBJECT);
	explosion->attachRigidBody(explosionBody);

	// Create the bomb object templates
	for (int i = 0; i < 4; i++)
	{
		std::shared_ptr<Bomb> bombObject = std::make_shared<Bomb>(
			glm::vec3(-100.0f),
			bombMesh,
			material,
			textures.at(i),
			i,
			explosion);

		// Create the rigidbody
		std::unique_ptr<RigidBody> rb = std::make_unique<RigidBody>(
			btBroadphaseProxy::SensorTrigger);
		rb->load(bodyPath);
		bombObject->attachRigidBody(rb);

		// Add the bomb to the templates
		bombTemplates.push_back(bombObject);
	}

	// Set the bomb outline texture
	bombTemplates[0]->setOutlineColour(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	bombTemplates[1]->setOutlineColour(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	initialized = true;
	return true;
}

void BombManager::setMaterialForAllBombs(std::shared_ptr<Material> mat)
{
	for (auto bomb : activeBombs)
	{
		bomb->setMaterial(mat);
	}
}

void BombManager::update(float dt)
{
	for (auto it : activeBombs)
	{
		it->update(dt);
	}

	// Keeps track of the bombs that are inactive
	static std::vector<std::vector<std::shared_ptr<Bomb>>::iterator> inactiveBombs;
	inactiveBombs.clear();

	// Gets the list of finished bombs
	for (auto it = activeBombs.begin(); it != activeBombs.end(); it++)
	{
		if (it->get()->getCurrentState() == DONE)
		{
			inactiveBombs.push_back(it);
		}
	}

	// Removes finished bombs from the game
	for (auto it : inactiveBombs)
	{
		activeBombs.erase(it);
	}
}

void BombManager::draw(Camera& camera)
{
	for (auto it : activeBombs)
	{
		it->draw(camera);
	}
}

void BombManager::throwBomb(Player* player, glm::vec2 direction, glm::vec3 force)
{
	if (!initialized) return;

	int playerNum = player->getPlayerNum();
	std::shared_ptr<Bomb> newBomb = std::make_shared<Bomb>(*bombTemplates.at(playerNum));
	newBomb->attachPlayerPtr(player);
	activeBombs.push_back(newBomb);
	newBomb->throwBomb(direction, force);
	
}



///////////////////////////////////////////////////////////////////////////////
////////////////////////////	BOMB	///////////////////////////////////////
float Bomb::playerRadius = 2.0f;
float Bomb::maxExplodeTime = 1.0f;
float Bomb::maxFuseTime = 3.0f;

Bomb::Bomb(glm::vec3 position,
	std::shared_ptr<Loader> _mesh,
	std::shared_ptr<Material> _material,
	std::shared_ptr<Texture> _texture,
	int _playerNum,
	std::shared_ptr<Explosion> _explosion)	
	: GameObject(position, _mesh, _material, _texture),
	playerNum(_playerNum),
	currentState(OFF),
	currentExplodeTime(0.0f),
	currentFuseTime(0.0f),
	explosion(_explosion)
{
	colliderType = COLLIDER_TYPE::BOMB_BASE;
	explosion->setBombParent(this);
}

Bomb::~Bomb()
{

}

Bomb::Bomb(Bomb& other)
	: GameObject(other),
	explosion(other.explosion),
	playerNum(other.playerNum),
	currentState(OFF),
	currentExplodeTime(0.0f),
	currentFuseTime(0.0f)
{
	colliderType = COLLIDER_TYPE::BOMB_BASE;
	explosion->setBombParent(this);
}

void Bomb::attachPlayerPtr(Player* _playerPtr)
{
	playerPtr = _playerPtr;
}

void Bomb::throwBomb(glm::vec2 direction, glm::vec3 force)
{
	setPosition(playerPtr->getWorldPosition() + glm::vec3(direction.x * playerRadius, 0.6f, -direction.y * playerRadius));
	currentFuseTime = maxFuseTime;
	currentState = THROWN;

	rigidBody->getBody()->applyCentralImpulse(
		btVector3(direction.x * force.x, force.y, -direction.y * force.z));
}



void Bomb::draw(Camera& camera)
{
	switch (currentState)
	{
	case OFF:
		break;
	case THROWN:
		GameObject::draw(camera);
		break;
	case EXPLODING:
		explosion->draw(camera);
		break;
	case DONE:
		break;
	default:
		break;
	}
}

void Bomb::update(float dt)
{
	switch (currentState)
	{
	case OFF:
		rigidBody->getBody()->clearForces();
		break;

	case THROWN:
		currentFuseTime -= dt;
		if (currentFuseTime <= 0.0f)
		{
			explode();
		}
		break;

	case EXPLODING:
		currentExplodeTime -= dt;
		if (currentExplodeTime <= 0.0f)
		{
			destroy();
		}
		rigidBody->getBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));	
		break;

	case DONE:
		break;
	default:
		break;
	}

	GameObject::update(dt);
	explosion->update(dt);
}

void Bomb::explode()
{
	currentFuseTime = 0.0f;
	currentExplodeTime = maxFuseTime;
	currentState = EXPLODING;
	std::cout << "Bomb " << playerNum << " exploded" << std::endl;

	// Swap the bomb and the explosion's position
	explosion->setPosition(getWorldPosition());
	setPosition(glm::vec3(-100.0f));
}

void Bomb::destroy()
{
	currentExplodeTime = 0.0f;
	currentState = DONE;
	std::cout << "Bomb " << playerNum << " destroyed" << std::endl;
	explosion->setPosition(glm::vec3(-100.0f));
}

void Bomb::setMaterial(std::shared_ptr<Material> _material)
{
	material = _material;
	explosion->setMaterial(_material);
}

Explosion::Explosion(glm::vec3 position,
	std::shared_ptr<Loader> _mesh,
	std::shared_ptr<Material> _material,
	std::shared_ptr<Texture> _texture,
	Bomb* _parent)
	: GameObject(position, _mesh, _material, _texture),
	parent(_parent)
{
	colliderType = COLLIDER_TYPE::BOMB_EXPLOSION;
	setScale(glm::vec3(5.0f));
}

Explosion::Explosion(Explosion& other)
	: GameObject(other),
	parent(nullptr)
{
	colliderType = COLLIDER_TYPE::BOMB_EXPLOSION;
	setScale(glm::vec3(5.0f));
}

void Explosion::update(float dt)
{

	GameObject::update(dt);
}

void Explosion::setBombParent(Bomb* newParent)
{
	parent = newParent;
}