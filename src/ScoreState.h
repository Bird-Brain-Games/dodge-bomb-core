#pragma once

#include "GameState.h"
#include <memory>
#include "menu.h"
#include "controller.h"

class Score : public GameState
{
public:
	Score(std::shared_ptr<Menu>);
	void setPaused(int);
	void update(float dt);
	void draw();
	Controller *active;
	std::shared_ptr<Menu> atlas;
private:
	float accept;
	float pauseTimer;
};