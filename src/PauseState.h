#pragma once

#include "GameState.h"
#include <memory>
#include "menu.h"
#include "controller.h"

class Pause : public GameState
{
public:
	Pause(std::shared_ptr<Menu>);
	void setPaused(int);
	void update(float dt);
	void draw();
	void setActive(Controller*);
	Controller *active;
	//used so we cant pause then immediately unpause.

private:
	float time;
	float timer;

	int position;
	//when we pause it passes the controller used so only that player can unpause
	std::shared_ptr<Menu> atlas;
};