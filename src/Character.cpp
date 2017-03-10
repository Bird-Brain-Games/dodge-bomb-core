#include "Character.h"
#include <random>
Character::Character()
{
	name = "Default";
	health[0] = 50;
	health[1] = 50;
	damage[0] = 5;
	damage[1] = 15;
	defense = 2;
}

Character::Character(std::string _name)
{
	name = _name;
	health[0] = 50;
	health[1] = 50;
	damage[0] = 5;
	damage[1] = 15;
	defense = 2;
}

Character::Character(std::string _name, int minH, int maxH, int minDmg, int maxDmg, int _defense)
{
	name = _name;
	health[0] = minH;
	health[1] = maxH;
	damage[0] = minDmg;
	damage[1] = maxDmg;
	defense = _defense;
}


void Character::attack(Character *enemy)
{
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 eng(rd()); // seed the generator
	std::uniform_int_distribution<> distr(damage[0], damage[1]); // define the range

	int attack = distr(eng);

	attack = (attack -= enemy->getDefense() <= 0) ? attack -= enemy->getDefense() : 1;
	
	enemy->health[0] = (enemy->health[0] - attack > 0) ? enemy->health[0] -= attack : 0;

	printf("%s %s %d %s %s %s", this->getName().c_str(), "did", attack, "damage to", enemy->getName().c_str(), "\n");
}

void Character::setMinDmg(int _dmg)
{
	damage[0] = _dmg;
}
int  Character::getMinDmg()
{
	return damage[0];
}

void Character::setMaxDmg(int _dmg)
{
	damage[1] = _dmg;
}
int  Character::getMaxDmg()
{
	return damage[1];
}

void Character::setHealth(int _health)
{
	health[0] = _health;
}
int  Character::getHealth()
{
	return health[0];
}

void Character::setDefense(int _defense)
{
	defense = _defense;
}

int  Character::getDefense()
{
	return defense;
}

void Character::setMaxH(int _health)
{
	health[1] = _health;
}

int  Character::getMaxH()
{
	return health[1];
}

std::string Character::getName()
{
	return name;
}