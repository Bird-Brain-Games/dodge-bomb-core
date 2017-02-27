#pragma once
#include "GameState.h"
#include "GameObject.h"
#include "menu.h"


#include "InputManager.h"

class MainMenu : public GameState
{
public:
	MainMenu(Menu*);

	void update(float dt);
	void draw();
	void setPaused(bool);
private:
	Menu *atlas;
};

class Pause : public GameState
{
public:
	Pause(Menu*);
	void setPaused(bool);
	void update(float dt);
	void draw();
	void setActive(Controller*);
	Controller *active;
	//used so we cant pause then immediately unpause.
	float pauseTimer;
private:
	//when we pause it passes the controller used so only that player can unpause
	Menu *atlas;
};

class Score : public GameState
{
public:
	Score(Menu*);
	void setPaused(bool);
	void update(float dt);
	void draw();
	Controller *active;
	Menu* atlas;
private:
	float pauseTimer;
};

class Game : public GameState
{
public:
	Game(std::vector<GameObject*>*, Pause*, Score*, Camera**);
	void update(float dt);//where we do all the updates and controls
	void draw();// where we do all 
private:
	std::vector<GameObject*> *objects;
	Pause* pause;
	Score* score;
	Camera ** camera;
	int pausing;
	float pauseTimer;
};


class Debug : public GameState
{
public:
	Debug(Camera*);
	void update(float dt);
	void draw();
private:
	Camera * camera;
};