#pragma once

#include "GameObject.h"
#include <btBulletDynamicsCommon.h>
#include <queue>

// Bomb emitter class tracks active bombs
// And assigns bomb properties upon creation

class Player;

enum BOMB_STATE
{
	OFF,
	THROWN,
	EXPLODING,
	DONE
};

class Explosion;

// Bomb object 
class Bomb : public GameObject
{
public:
	Bomb(glm::vec3 position,
		std::shared_ptr<Loader> _mesh,
		std::shared_ptr<Material> _material,
		std::shared_ptr<Texture> _texture,
		int _playerNum,
		std::shared_ptr<Explosion> explosion);
	Bomb(Bomb&);
	~Bomb();

	void attachPlayerPtr(Player* _playerPtr);
	void draw(Camera& camera);
	void update(float dt);
	void setMaterial(std::shared_ptr<Material> _material);

	void throwBomb(glm::vec2 direction, glm::vec3 force);
	void explode();
	void destroy();
	BOMB_STATE getCurrentState() { return currentState; }

	Player const* getPlayer() { return playerPtr; }
	int getPlayerNum() { return playerNum; }
	
	bool justExploded;

private:
	static float playerRadius;
	float duration;
	int playerNum;
	Player* playerPtr;
	std::shared_ptr<Explosion> explosion;

	// Bomb stats
	BOMB_STATE currentState;
	float currentFuseTime;
	float currentExplodeTime;
	

	static float maxFuseTime;
	static float maxExplodeTime;
};

class Explosion : public GameObject
{
public:
	Explosion(glm::vec3 position,
		std::shared_ptr<Loader> _mesh,
		std::shared_ptr<Material> _material,
		std::shared_ptr<Texture> _texture,
		Bomb* _parent);
	Explosion(Explosion&);
	//~Explosion();

	//void draw(Camera& camera);
	void update(float dt);
	void setBombParent(Bomb*);
	Bomb* getBombParent() { return parent; }

	void explode();

private:
	Bomb* parent;
	float expandTimer;
	bool isExpanding;

private:
	static float timeToExpand;
	static float minScale;
	static float maxScale;
};

/*
holds 4 bomb objects, 1 for each player
when player asks to throw bomb, sends call to emitter
with player number, and the implulse, and creates
the game object using copy constructor
*/
class BombManager
{
public:
	BombManager();
	bool init(
		std::shared_ptr<Loader> _bombMesh,
		std::shared_ptr<Texture> _p1,
		std::shared_ptr<Texture> _p2,
		std::shared_ptr<Texture> _p3,
		std::shared_ptr<Texture> _p4,
		std::shared_ptr<Loader> _explosionMesh,
		std::shared_ptr<Texture> _explosionTex,
		std::string _explosionBodyPath,
		std::shared_ptr<Material> _material,
		std::string bodyPath);

	void update(float dt);
	void checkIfExploded(Camera& camera);
	void draw(Camera& camera);
	void setMaterialForAllBombs(std::shared_ptr<Material> mat);

	// Create the new bomb, add it to the active list,
	// and tell it to throw in the given direction.
	void throwBomb(Player*, glm::vec2 direction, glm::vec2 playerVelocity, glm::vec3 force);

private:
	std::shared_ptr<Loader> bombMesh, explosionMesh;
	std::shared_ptr<Material> material;
	std::vector <std::shared_ptr<Texture>> textures;
	std::vector <std::shared_ptr<Bomb>> bombTemplates;
	std::vector <std::shared_ptr<Bomb>> activeBombs;
	std::queue<std::shared_ptr<Bomb>> bombQueue;
	std::shared_ptr<Explosion> explosion;

	bool initialized;
	float impulseY;
};