#include "PauseState.h"
#include "FrameBufferObject.h"

Pause::Pause(std::shared_ptr<Menu> _atlas)
{
	atlas = _atlas;
	position = 6;
}

void Pause::update(float dt)
{
	time += dt;
	timer += dt;



	switch (position)
	{
		if (timer > 0.35)
		{
	case 6:
		if (active->conButton(XINPUT_GAMEPAD_A))
		{
			timer = 0;
			setPaused(true);
			m_parent->getGameState("MainMenu")->setPaused(0);
			m_parent->getGameState("game")->setPaused(-1);
		}
		break;
	case 4:
		if (active->conButton(XINPUT_GAMEPAD_A))
		{
			timer = 0;
			setPaused(true);
			m_parent->getGameState("game")->setPaused(0); // resets the players by passing in -1.
		}
		break;
		}
	}

	Coords rStick = active->getLeftStick();
	if (rStick.y > 0 && position < 6 && timer > 0.35)
	{
		position++;
		atlas->setSpot(0, position);
		timer = 0;
	}

	else if (rStick.y < 0 && position > 4 && timer > 0.35)
	{
		position--;
		atlas->setSpot(0, position);
		timer = 0;
	}

	if (time > 0.30)
	{
		atlas->incSpotR();
		time = 0;
	}


}
void Pause::draw()
{
	FrameBufferObject::clearFrameBuffer(glm::vec4(0.0, 0.0, 0.0, 0.0));
	atlas->draw();
}

void Pause::setActive(Controller* _con)
{
	active = _con;
}

void Pause::setPaused(int _state)
{
	m_isPaused = _state;
	if (!_state)
	{
		position = 6;
		atlas->setSpot(glm::vec2(0, position));
	}
}