#include <string>
class Character
{
public:
	Character();
	Character(std::string);
	Character(std::string, int, int, int, int, int);
	void attack(Character*);

	void setMinDmg(int);
	int getMinDmg();

	void setMaxDmg(int);
	int getMaxDmg();

	void setHealth(int);
	int getHealth();

	void setDefense(int);
	int getDefense();

	void setMaxH(int);
	int getMaxH();

	std::string getName();

private:
	// 0 = Health, 1 = Max Health 
	int health[2];
	std::string name;
	int damage[2];
	int defense;
};