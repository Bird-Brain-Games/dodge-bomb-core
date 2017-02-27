#pragma once

#include <vector>
#include <string>
#include <unordered_map>//essentially a vector/array that we have more control over. 

class GameState;

class GameStateManager
{
public:
	void addGameState(std::string a_string, GameState* a_state);//adds a game state to the list of game states
	void removeGameState(std::string a_string);//removes the specified game state from the list
	GameState* getGameState(std::string a_string);//returns a pointer to the gamestate that we specifiy
	void update(float dt);//calls the update functions for the game states
	void draw();


private:
	std::unordered_map<std::string, GameState*> m_states;//essentially a vector/array we have more control over
	//when we add a variable to it we specify a string. that string is how we call that variable. 
	//if we gave it the string "main" then to call it instead of 0 we would give it main
};

class GameState
{
public:

	virtual void update(float dt) = 0;//this is a function that doesnt exsist in this class but can be defined by those that it is inherited from.
	virtual void draw() = 0;//this is a function that doesnt exsist in this class but can be defined by those that it is inherited from.
	void setPaused(bool a_paused);//this controls whether a game state is paused or not. When paused it doesnt update
	bool m_isPaused = false;//the variable that controls whether the game state is paused. it starts unpaused
	GameStateManager* m_parent;// a pointer the the game state manager.
	
private:
};