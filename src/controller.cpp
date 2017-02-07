#include "controller.h"
#include <iostream>
#include <math.h>
#define MAXVALUE 32768

//#magic stuff. dont touch.

Controller::Controller(int ControllerIndex)
{
	m_ControllerHandle = ControllerIndex;
}

bool Controller::connected()
{
	ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));
	
	DWORD Result = XInputGetState(m_ControllerHandle, &m_CurrentState);
	if (Result == ERROR_SUCCESS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

XINPUT_STATE Controller::getState()
{
	ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));

	XInputGetState(m_ControllerHandle, &m_CurrentState);

	return m_CurrentState;
}

Coords Controller::getRightStick()
{
	ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));
	XInputGetState(m_ControllerHandle, &m_CurrentState);

	Coords result;
	if (
		(int)sqrt((float)
		(m_CurrentState.Gamepad.sThumbRY * m_CurrentState.Gamepad.sThumbRY)
		+ 
		(m_CurrentState.Gamepad.sThumbRX * m_CurrentState.Gamepad.sThumbRX)) 
		> XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		result.x = (float)m_CurrentState.Gamepad.sThumbRX / (float)MAXVALUE;
		result.y = (float)m_CurrentState.Gamepad.sThumbRY / (float)MAXVALUE;
	}
	else
	{
		result.x = 0;
		result.y = 0;
	}
	return result;
}

Coords Controller::getLeftStick()
{
	ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));
	XInputGetState(m_ControllerHandle, &m_CurrentState);


	Coords result;
	if ((int)sqrt((float)(m_CurrentState.Gamepad.sThumbLY * m_CurrentState.Gamepad.sThumbLY) + (m_CurrentState.Gamepad.sThumbLX * m_CurrentState.Gamepad.sThumbLX)) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		result.x = (float)m_CurrentState.Gamepad.sThumbLX / (float)MAXVALUE;
		result.y = (float)m_CurrentState.Gamepad.sThumbLY / (float)MAXVALUE;
	}
	else
	{
		result.x = 0;
		result.y = 0;
	}
	return result;

}

bool Controller::conButton(int button)
{
	if (getState().Gamepad.wButtons & button)
	{
		return true;
	}
	return false;
}

float Controller::conRightTrigger()
{
	float state = getState().Gamepad.bRightTrigger;
	return state / 255;
}

float Controller::conLeftTrigger()
{
	float state = getState().Gamepad.bLeftTrigger;
	return state / 255;
}

Controller::~Controller()
{

}