#include "MainMenuState.h"

#include "gl\freeglut.h"
#include "InputManager.h"
#include "FrameBufferObject.h"


bool isPlaying = false;

void MainMenu::update(float dt)
{
	time += dt;
	timer += dt;
	accept += dt;
	

	if (accept > 0.5)
	{
		switch (position)
		{
		case 6:
			if (con->conButton(XINPUT_GAMEPAD_A))
			{
				audio->playSounds(3); //Play select sound

				isPlaying = false;
				audio->pauseSounds(1); //Turns off Music

				

				setPaused(true);
				m_parent->getGameState("game")->setPaused(-1); // resets the players by passing in two.
				
			}
			break;
		case 3:
			if (con->conButton(XINPUT_GAMEPAD_A))
			{
				audio->playSounds(3); //Play select sound
				glutLeaveMainLoop();
			}
			break;
		}
	}
	if (KEYBOARD_INPUT->CheckPressEvent(13))
	{
		isPlaying = false;
		audio->pauseSounds(1); //Turns off Music

		setPaused(true);
		m_parent->getGameState("game")->setPaused(-1); // resets the players by passing in two.
	}

	Coords rStick = con->getLeftStick();
	if (rStick.y > 0 && position < 6 && timer > 0.35)
	{
		position++;
		atlas->setSpot(0, position);
		timer = 0;
		audio->playSounds(2); //Play switch sound
	}
	else if (rStick.y < 0 && position > 3 && timer > 0.35)
	{
		position--;
		atlas->setSpot(0, position);
		timer = 0;
		audio->playSounds(2); //Play switch sound
	}

	if (time > incrememnt)
	{
		atlas->incSpotR();
		time = 0;
	}
}

void MainMenu::draw()
{
	FrameBufferObject::clearFrameBuffer(glm::vec4(0.3, 0.0, 0.0, 1.0));
	atlas->draw();
}

MainMenu::MainMenu(std::shared_ptr<Menu> _atlas, Controller* _con, SoundDriver* _audio)
{
	atlas = _atlas;
	position = 6;
	frame = 0;
	atlas->setSpot(0, position);
	time = 0;
	timer = 0;
	incrememnt = 0.3;
	accept = 1;
	con = _con;
	audio = _audio;
}

void MainMenu::setPaused(int _state)
{
	m_isPaused = _state;
	if (!_state)// set this state to play
	{
		atlas->setSpot(glm::vec2(0, position));
		accept = 0;

		audio->playSounds(1);
	}
}