#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Xinput.h>

//#magic stuff

struct Coords
{
	float x, y;
};

class Controller
{
public:
	~Controller();
	Controller(int ControllerIndex);//the constructor. ControllerIndex is the Controller number.

	bool connected();//checks whether the Controller is plugged in or not.
	int getPlayerNum() { return m_ControllerHandle; }

	Coords getLeftStick();//gets the right stick. returns a coord variable (a structure with two floats in it. x and y)
	Coords getRightStick();//gets the left stick. returns a coord variable (a structure with two floats in it. x and y)

	bool rightStickMoved();
	bool leftStickMoved();

	/**
	*XINPUT_GAMEPAD_DPAD_UP		0x0001
	*XINPUT_GAMEPAD_DPAD_DOWN	0x0002
	*XINPUT_GAMEPAD_DPAD_LEFT	0x0004
	*XINPUT_GAMEPAD_DPAD_RIGHT	0x0008
	*XINPUT_GAMEPAD_START	0x0010
	*XINPUT_GAMEPAD_BACK	0x0020
	*XINPUT_GAMEPAD_LEFT_THUMB	0x0040
	*XINPUT_GAMEPAD_RIGHT_THUMB	0x0080
	*XINPUT_GAMEPAD_LEFT_SHOULDER	0x0100
	*XINPUT_GAMEPAD_RIGHT_SHOULDER	0x0200
	*XINPUT_GAMEPAD_A	0x1000
	*XINPUT_GAMEPAD_B	0x2000
	*XINPUT_GAMEPAD_X	0x4000
	*XINPUT_GAMEPAD_Y	0x8000
	*/
	bool conButton(int);//use this to get whether a button is pressed down or not. you need to give it an enum from above this

	float conRightTrigger();//these are return a value from 0 to 1 when a trigger is pressed. 1 is a complete press and 0 is nothing.
	float conLeftTrigger();// these are return a value from 0 to 1 when a trigger is pressed. 1 is a complete press and 0 is nothing.

	XINPUT_STATE getState();
private:
	XINPUT_STATE m_CurrentState;
	DWORD m_ControllerHandle;
};


