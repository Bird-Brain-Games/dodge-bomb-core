// InputManager.h
// - Keyboard input handler given by TA  (2015)
// - Simple class to handle input handling
// - The content of this is somewhat beyond the
//   scope of this course, don't worry too much about it
// - Just use it

#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <map>
#include <vector>

#define KEYBOARD_INPUT InputManager::GetPtr()

/* Simple class to handle input handling
	The content of this is somewhat beyond the 
	scope of this course, don't worry too much about it
	Just use it*/
class InputManager
{
private:
	std::map<unsigned char, bool> inputList;
	std::vector<std::pair<unsigned char, bool>> eventList;
	
	static InputManager* instance;
	InputManager();
	~InputManager();
public:

	static InputManager* GetPtr();

	// Set whether a key has been pressed (true) or released (false)
	void SetActive(unsigned char key, bool state);

	// Check to see if a key is currently down
	bool IsKeyDown(unsigned char key);

	// Check if a keypress event happened
	bool CheckPressEvent(unsigned char key);

	// Check if a key release event happened
	bool CheckReleaseEvent(unsigned char key);

	// Do this every frame after processing press and release events
	void WipeEventList();


	void Destroy();

};

#endif
