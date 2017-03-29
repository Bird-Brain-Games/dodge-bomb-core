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
	case 5:
		if (active->conButton(XINPUT_GAMEPAD_A))
		{
			setPaused(true);
			m_parent->getGameState("MainMenu")->setPaused(0);
		}
		break;
	case 3:
		if (active->conButton(XINPUT_GAMEPAD_A))
		{
			setPaused(true);
			m_parent->getGameState("game")->setPaused(0); // resets the players by passing in -1.
		}
		break;
	}

	Coords rStick = active->getLeftStick();
	if (rStick.y > 0 && position < 5 && timer > 0.35)
	{
		position++;
		atlas->setSpot(0, position);
		timer = 0;
	}
	else if (rStick.y < 0 && position > 3 && timer > 0.35)
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
	FrameBufferObject::clearFrameBuffer(glm::vec4(0.3, 0.0, 0.0, 1.0));
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