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
	Sound::sys.update();
	

	if (accept > 0.5)
	{
		switch (position)
		{
		case 6:
			if (con->conButton(XINPUT_GAMEPAD_A))
			{
				s_menuSelect.play();

				isPlaying = false;
				m_mainMenu.pause();

				

				setPaused(true);
				m_parent->getGameState("game")->setPaused(-1); // resets the players by passing in two.
				
			}
			break;
		case 3:
			if (con->conButton(XINPUT_GAMEPAD_A))
			{
				s_menuSelect.play();
				glutLeaveMainLoop();
			}
			break;
		}
	}
	if (KEYBOARD_INPUT->CheckPressEvent(13))
	{
		isPlaying = false;
		m_mainMenu.pause();

		setPaused(true);
		m_parent->getGameState("game")->setPaused(-1); // resets the players by passing in two.
	}

	Coords rStick = con->getLeftStick();
	if (rStick.y > 0 && position < 6 && timer > 0.35)
	{
		position++;
		atlas->setSpot(0, position);
		timer = 0;
		s_menuSwitch.play();
	}
	else if (rStick.y < 0 && position > 3 && timer > 0.35)
	{
		position--;
		atlas->setSpot(0, position);
		timer = 0;
		s_menuSwitch.play();
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

MainMenu::MainMenu(std::shared_ptr<Menu> _atlas, Controller* _con, std::map<std::string, Sound>* _soundTemplates)
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
	soundTemplates = _soundTemplates;

//	m_mainMenu = Sound(soundTemplates->at("m_mainMenu"));
//	m_mainMenu.setPosition(glm::vec3(23.0f, 45.0f, 25.0f));
//	//m_mainMenu.setRolloff(true, 0.5, 20.0f);
	s_menuSelect = Sound(soundTemplates->at("s_menuSelect"));
	s_menuSelect.setPosition(glm::vec3(23.0f, 45.0f, 25.0f));
	//s_menuSelect.setRolloff(true, 0.5, 20.0f);
	s_menuSwitch = Sound(soundTemplates->at("s_menuSwitch"));
	s_menuSwitch.setPosition(glm::vec3(23.0f, 45.0f, 25.0f));
	//s_menuSwitch.setRolloff(true, 0.5, 20.0f);
}

void MainMenu::setPaused(int _state)
{
	m_isPaused = _state;
	if (!_state)// set this state to play
	{
		atlas->setSpot(glm::vec2(0, position));
		accept = 0;

		m_mainMenu.play();
	}
}