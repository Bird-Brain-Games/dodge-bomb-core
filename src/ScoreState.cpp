#include "ScoreState.h"
#include "FrameBufferObject.h"

Score::Score(std::shared_ptr<Menu> _atlas)
{
	atlas = _atlas;
}

void Score::setPaused(int _state)
{

	if (_state > 1 || _state == 0)
	{
		m_isPaused = false;
		switch (_state)
		{
		case 2:
			atlas->setSpot(0, 5);
			break;
			
		case 3:
			atlas->setSpot(0, 6);
			break;

		case 4:
			atlas->setSpot(0, 4);
			break;

		case 5:
			atlas->setSpot(0, 3);
			break;
		}

		accept = 0;
	}
	else if (_state == 1)
		m_isPaused = true;
}

void Score::draw()
{
	FrameBufferObject::clearFrameBuffer(glm::vec4(0.3, 0.0, 0.0, 1.0));
	atlas->draw();
}

void Score::update(float dt)
{
	pauseTimer += dt;
	accept += dt;
	if (accept > 0.45)
	{
		if (active->conButton(XINPUT_GAMEPAD_A))
		{
			setPaused(true);
			m_parent->getGameState("MainMenu")->setPaused(0);
			pauseTimer = 0;
		}
	}

	if (pauseTimer > 0.3)
	{
		atlas->incSpotR();
		pauseTimer = 0;
	}
}