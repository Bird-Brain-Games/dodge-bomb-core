#pragma once
#include "GameState.h"
#include "GameObject.h"
#include "menu.h"
class MainMenu : public GameState
{
public:
	MainMenu(Menu*);

	void update();
	void draw();
private:
	Menu *atlas;
};

class Pause : public GameState
{
public:
	void update();
	void draw();
private:
	//when we pause it passes the controller used so only that player can unpause
	Controller *active;
};

class Game : public GameState
{
public:
	Game(std::vector<GameObject*>*, Pause*);
	void update();
	void draw();
private:
	std::vector<GameObject*> *objects;
};


