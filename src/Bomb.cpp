#include "Bomb.h"

Bomb::Bomb(glm::vec3 position,
	std::shared_ptr<Loader> _mesh,
	std::shared_ptr<Material> _material,
	std::shared_ptr<Texture> _texture,
	int playerNum)
	: GameObject(position, _mesh, _material, _texture)
{

}

Bomb::~Bomb()
{

}