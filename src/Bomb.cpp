#include "Bomb.h"
#include "Player.h"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
////////////////////////	BOMB MANAGER	///////////////////////////////////
BombManager::BombManager()
{
	impulseY = 35.0f;
	initialized = false;
}

bool BombManager::init(
	std::shared_ptr<Loader> _bombMesh,
	std::shared_ptr<Texture> _p1,
	std::shared_ptr<Texture> _p2,
	std::shared_ptr<Texture> _p3,
	std::shared_ptr<Texture> _p4,
	std::shared_ptr<Loader> _explosionMesh,
	std::shared_ptr<Texture> _explosionTex1,
	std::shared_ptr<Texture> _explosionTex2,
	std::shared_ptr<Texture> _explosionTex3,
	std::shared_ptr<Texture> _explosionTex4,
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
	textures.push_back(_explosionTex1);
	textures.push_back(_explosionTex2);
	textures.push_back(_explosionTex3);
	textures.push_back(_explosionTex4);

	// Create explosion objects
	for (int i = 0; i < 4; i++)
	{
		std::shared_ptr<Explosion> explosionObject = std::make_shared<Explosion>(
			glm::vec3(-100.0f),
			explosionMesh,
			material,
			textures[i + 4],
			nullptr);

		// Create the explosion rigidBody
		std::unique_ptr<RigidBody> explosionBody =
			std::make_unique<RigidBody>(btBroadphaseProxy::DebrisFilter);
		explosionBody->load(_explosionBodyPath, btCollisionObject::CF_KINEMATIC_OBJECT);
		explosionObject->attachRigidBody(explosionBody);

		explosionTemplates.push_back(explosionObject);
	}

	// Create the bomb object templates
	for (int i = 0; i < 4; i++)
	{
		std::shared_ptr<Bomb> bombObject = std::make_shared<Bomb>(
			glm::vec3(-100.0f),
			bombMesh,
			material,
			textures.at(i),
			i,
			explosionTemplates.at(i));

		// Create the rigidbody
		std::unique_ptr<RigidBody> rb = std::make_unique<RigidBody>(
			btBroadphaseProxy::SensorTrigger);
		rb->load(bodyPath);
		bombObject->attachRigidBody(rb);

		// Add the bomb to the templates
		bombTemplates.push_back(bombObject);
	}

	// Set the bomb outline texture
	bombTemplates[0]->setOutlineColour(glm::vec4(0.39f, 0.72f, 1.0f, 1.0f));
	bombTemplates[1]->setOutlineColour(glm::vec4(1.0f, 0.41f, 0.37f, 1.0f));
	bombTemplates[2]->setOutlineColour(glm::vec4(0.31f, 0.93f, 0.32f, 1.0f));
	bombTemplates[3]->setOutlineColour(glm::vec4(0.88f, 0.87f, 0.33f, 1.0f));

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

	// Pop off inactive bombs
	while (activeBombs.size() > 0 &&
		activeBombs.back()->getCurrentState() == DONE)
	{
		activeBombs.pop_back();
	}
}

void BombManager::draw(Camera& camera)
{
	for (auto it : activeBombs)
	{
		if (it->getCurrentState() != BOMB_STATE::OFF ||
			it->getCurrentState() != BOMB_STATE::DONE)
			it->draw(camera);
	}
}

void BombManager::checkIfExploded(Camera& camera)
{
	for (auto it : activeBombs)
	{
		if (it->justExploded)
		{
			camera.shakeScreen(1.0f);
			it->justExploded = false;
		}
	}
}

void BombManager::throwBomb(Player* player, glm::vec2 direction, glm::vec2 playerVelocity, glm::vec3 force)
{
	if (!initialized) return;

	int playerNum = player->getPlayerNum();
	std::shared_ptr<Bomb> newBomb = std::make_shared<Bomb>(*bombTemplates.at(playerNum));
	newBomb->attachPlayerPtr(player);
	activeBombs.push_back(newBomb);

	glm::vec2 playerForce;
	if (playerVelocity != glm::vec2(0.0f))
	{
		playerForce = glm::normalize(playerVelocity) * glm::abs(playerVelocity);
	}
	else
	{
		playerForce = glm::vec2(0.0f);
	}

	// direction of bomb * bomb force + direction of player * player force
	force = glm::vec3(direction.x, 0.0f, -direction.y) * force +
		glm::vec3(playerForce.x, 2.0f, -playerForce.y) * 0.5f;
	std::cout << "Force vector: " << force.x << " " << force.z << std::endl;

	newBomb->throwBomb(direction, glm::vec3(force.x, impulseY, force.z));
}

void BombManager::clearAllBombs()
{
	activeBombs.clear();
}


///////////////////////////////////////////////////////////////////////////////
////////////////////////////	BOMB	///////////////////////////////////////
float Bomb::playerRadius = 2.0f;
float Bomb::maxExplodeTime = 0.45f;
float Bomb::maxFuseTime = 1.3f;

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
	explosion(_explosion),
	justExploded(false)
{
	colliderType = COLLIDER_TYPE::BOMB_BASE;
	explosion->setBombParent(this);
	setScale(glm::vec3(3.0f));
}

Bomb::~Bomb()
{

}

Bomb::Bomb(Bomb& other)
	: GameObject(other),
	explosion(std::make_shared<Explosion>(*other.explosion)),
	playerNum(other.playerNum),
	currentState(OFF),
	currentExplodeTime(0.0f),
	currentFuseTime(0.0f),
	justExploded(false)
{
	colliderType = COLLIDER_TYPE::BOMB_BASE;
	explosion->setBombParent(this);
	setScale(glm::vec3(3.0f));
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
		btVector3(force.x, force.y, force.z));
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
	currentExplodeTime = maxExplodeTime;
	currentState = EXPLODING;
	std::cout << "Bomb " << playerNum << " exploded" << std::endl;

	// Swap the bomb and the explosion's position
	explosion->setPosition(getWorldPosition());
	explosion->explode();
	setPosition(glm::vec3(-100.0f));
	justExploded = true;
}

void Bomb::destroy()
{
	currentExplodeTime = 0.0f;
	currentState = DONE;
	std::cout << "Bomb " << playerNum << " destroyed" << std::endl;
	explosion->setPosition(glm::vec3(-100.0f));
	explosion->setBombParent(nullptr);
}

void Bomb::setMaterial(std::shared_ptr<Material> _material)
{
	material = _material;
	explosion->setMaterial(_material);
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////	EXPLOOOOSION	///////////////////////////////////
float Explosion::timeToExpand = 0.15f;
float Explosion::maxScale = 7.5f;
float Explosion::minScale = 0.5f;

Explosion::Explosion(glm::vec3 position,
	std::shared_ptr<Loader> _mesh,
	std::shared_ptr<Material> _material,
	std::shared_ptr<Texture> _texture,
	Bomb* _parent)
	: GameObject(position, _mesh, _material, _texture),
	expandTimer(timeToExpand),
	parent(_parent)
{
	colliderType = COLLIDER_TYPE::BOMB_EXPLOSION;
	//setScale(glm::vec3(maxScale));
}

Explosion::Explosion(Explosion& other)
	: GameObject(other),
	expandTimer(timeToExpand),
	parent(nullptr)
{
	colliderType = COLLIDER_TYPE::BOMB_EXPLOSION;
	//setScale(glm::vec3(maxScale));
}

void Explosion::update(float dt)
{
	if (expandTimer < timeToExpand)
	{
		expandTimer += dt;
		if (expandTimer > timeToExpand)
		{
			expandTimer = timeToExpand;

		}
		setScale(glm::vec3(minScale + 
			(expandTimer / timeToExpand) * (maxScale - minScale)));
	}

	GameObject::update(dt);
}

void Explosion::setBombParent(Bomb* newParent)
{
	parent = newParent;
}

void Explosion::explode()
{
	expandTimer = 0.0f;
	rigidBody->setDeactivationMode(ISLAND_SLEEPING);
}