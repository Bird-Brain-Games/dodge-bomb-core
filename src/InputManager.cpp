// InputManager.cpp
// - Keyboard input handler given by TA Thomas (2015)
// - Implementation file of InputManager.cpp
// - The content of this is somewhat beyond the
//   scope of this course, don't worry too much about it
// - Just use it

#include "InputManager.h"
#ifdef KEY_EVENT_DISPLAY // define KEY_EVENT_DISPLAY somewhere in your code if you want to cout inputs
#include <iostream>
#endif

/* Instantiate our Input Manager */
InputManager *InputManager::instance = NULL;

InputManager::InputManager()
{

}

InputManager::~InputManager()
{
	//inputList.clear();
}

void InputManager::Destroy()
{
	if (instance != NULL)
	{
		delete instance;
		instance = NULL;
	}
}

InputManager* InputManager::GetPtr()
{
	if (instance == NULL)
		instance = new InputManager();
	return instance;
}

// Set whether a key has been pressed (true) or released (false)
void InputManager::SetActive(unsigned char key, bool state)
{
	inputList[key] = state;

	eventList.push_back(std::make_pair(key, state));
}

// Check to see if a key is currently down
bool InputManager::IsKeyDown(unsigned char key)
{
	return inputList[key];
}

// Check if a keypress event happened
bool InputManager::CheckPressEvent(unsigned char key)
{
	for (unsigned int i = 0; i < eventList.size(); ++i)
	{
		if (eventList[i].first == key)
		{
			if (eventList[i].second == true) return true;
			else return false;
		}
	}
	return false;
}

// Check if a key release event happened
bool InputManager::CheckReleaseEvent(unsigned char key)
{
	for (unsigned int i = 0; i < eventList.size(); ++i)
	{
		if (eventList[i].first == key)
		{
			if (eventList[i].second == false) return true;
			else return false;
		}
	}
	return false;
}

// Do this every frame after processing press and release events
void InputManager::WipeEventList()
{
#ifdef KEY_EVENT_DISPLAY
	for (unsigned int i = 0; i < eventList.size(); ++i)
	{
		if (eventList[i].second == true)
		{
			std::cout << "Key " << (int)eventList[i].first << "(" << eventList[i].first << ") was Pressed" << std::endl;
		}
		else
		{
			std::cout << "Key " << (int)eventList[i].first << "(" << eventList[i].first << ") was Released" << std::endl;
		}
	}
#endif

	eventList.clear();
}