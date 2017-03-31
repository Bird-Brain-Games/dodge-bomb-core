#pragma once

#include "GameState.h"
#include <memory>
#include "menu.h"
#include "controller.h"
#include "SoundDriver.h"

class MainMenu : public GameState
{
public:
	MainMenu(std::shared_ptr<Menu>, Controller*, SoundDriver*);

	void update(float dt);
	void draw();
	void setPaused(int);

private:
	Controller* con;
	float accept;
	float time;
	float timer;
	float incrememnt;
	int position, frame;
	std::shared_ptr<Menu> atlas;
	SoundDriver* audio;
};