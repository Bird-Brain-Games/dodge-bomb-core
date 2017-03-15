#pragma once

#include "GameState.h"
#include <memory>
#include "menu.h"

class MainMenu : public GameState
{
public:
	MainMenu(std::shared_ptr<Menu>);

	void update(float dt);
	void draw();
	void setPaused(int);

private:
	float accept;
	float time;
	float timer;
	float incrememnt;
	int position, frame;
	std::shared_ptr<Menu> atlas;
};