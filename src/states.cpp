#include "states.h"
#include "controller.h"
Controller menus(0);

void MainMenu::update()
{
	if (menus.conButton(XINPUT_GAMEPAD_A))
	{
		setPaused(true);
		m_parent->getGameState("game")->setPaused(false);
	}
}

void MainMenu::draw()
{
	atlas->draw();
}

MainMenu::MainMenu(Menu* _atlas)
{
	atlas = _atlas;
}

void Game::update()
{

}
void Game::draw()
{

}

void Pause::update()
{

}
void Pause::draw()
{

}
