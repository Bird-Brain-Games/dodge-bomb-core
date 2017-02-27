#include "states.h"
#include "controller.h"

#include "RigidBody.h"

Controller menus1(0);
Controller menus2(1);
Controller menus3(2);
Controller menus4(3);

void MainMenu::update(float dt)
{
	if (menus1.conButton(XINPUT_GAMEPAD_A))
	{
		setPaused(true);
		m_parent->getGameState("game")->setPaused(false);
	}
}
//returns a number based on who pressed a key. returns 0 if no key pressed
int allPress(int button)
{
	if (menus1.conButton(button))
	{
		return 1;
	}
	else if (menus2.conButton(button))
	{
		return 2;
	}
	else if (menus3.conButton(button))
	{
		return 3;
	}
	else if (menus4.conButton(button))
	{
		return 4;
	}
	else
		return 0;
}

void MainMenu::draw()
{
	atlas->draw();
}

MainMenu::MainMenu(Menu* _atlas)
{
	atlas = _atlas;
}

void MainMenu::setPaused(bool _state)
{
	m_isPaused = _state;
	if (!_state)
		atlas->setSpot(0, 0);
}

Game::Game(std::vector<GameObject*>* _object, Pause* _pause, Score* _score, Camera** _camera)
{
	pause = _pause;
	objects = _object;
	score = _score;
	camera = _camera;
	pauseTimer = 2.0;
}

void Game::update(float dt)
{
	pauseTimer += dt;
	for (unsigned int i = 0; i < objects->size(); i++)
	{
		objects->at(i)->update(dt);
	}
	pausing = allPress(XINPUT_GAMEPAD_START);
	if (pausing > 0 && pauseTimer > 1.0)
	{
		switch (pausing)
		{
		case 0:
			break;
		case 1:
			setPaused(true);
			pause->setPaused(false);
			pause->active = &menus1;
			pauseTimer = 0;
			break;
		case 2:
			setPaused(true);
			pause->setPaused(false);
			pause->active = &menus2;
			pauseTimer = 0;
			break;
		case 3:
			setPaused(true);
			pause->setPaused(false);
			pause->active = &menus3;
			pauseTimer = 0;
			break;
		case 4:
			setPaused(true);
			pause->setPaused(false);
			pause->active = &menus4;
			pauseTimer = 0;
			break;
		}
	}
	if (pausing = allPress(XINPUT_GAMEPAD_BACK))
	{
		//reset all the positions and everything.
		setPaused(true);
		score->setPaused(false);
		score->active = &menus1;
		pauseTimer = 0;
		if (pausing <= 2)
			score->atlas->setSpot(0, 1);
		else
			score->atlas->setSpot(1, 0);
	}

}

void Game::draw()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	for (unsigned int i = 0; i < objects->size(); i++)
	{
		objects->at(i)->draw(**camera);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

Pause::Pause(Menu *_atlas)
{
	atlas = _atlas;
}

void Pause::update(float dt)
{
	pauseTimer += dt;
	if (active->conButton(XINPUT_GAMEPAD_START) && pauseTimer > 1.0)
	{
		setPaused(true);
		m_parent->getGameState("game")->setPaused(false);
		pauseTimer = 0;
	}

}
void Pause::draw()
{
	atlas->draw();
}
void Pause::setActive(Controller* _con)
{
	active = _con;
}

void Pause::setPaused(bool _state)
{
	m_isPaused = _state;
	if (!_state)
		atlas->setSpot(1, 1);
}

Score::Score(Menu* _atlas)
{
	atlas = _atlas;
}

void Score::setPaused(bool _state)
{
	m_isPaused = _state;
}

void Score::draw()
{
	atlas->draw();
}

void Score::update(float dt)
{
	pauseTimer += dt;
	if (active->conButton(XINPUT_GAMEPAD_A) && pauseTimer > 1.0)
	{
		setPaused(true);
		m_parent->getGameState("main menu")->setPaused(false);
		pauseTimer = 0;
	}
}

Debug::Debug(Camera* _camera)
{
	camera = _camera;
}

void Debug::update(float dt)
{
	if (KEYBOARD_INPUT->CheckPressEvent('e') || KEYBOARD_INPUT->CheckPressEvent('E'))
	{
		RigidBody::setDebugDraw(!RigidBody::isDrawingDebug());
	}
}

void Debug::draw()
{
	if (RigidBody::isDrawingDebug())
		RigidBody::drawDebug(camera->getView(), camera->getProj());
}