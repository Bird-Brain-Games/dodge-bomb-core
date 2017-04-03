#pragma once

#include "GameState.h"
#include <memory>
#include "menu.h"
#include "controller.h"

class MainMenu : public GameState
{
public:
	MainMenu(std::shared_ptr<Menu>, Controller*, std::map<std::string, Sound>* soundTemplates);

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
	std::map<std::string, Sound>* soundTemplates;

private:
	// Sounds
	Sound m_mainMenu, s_menuSelect, s_menuSwitch;
};