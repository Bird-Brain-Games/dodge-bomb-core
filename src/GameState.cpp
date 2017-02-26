#include "GameState.h"
#include <iostream>
void GameStateManager::addGameState(std::string a_string, GameState* a_state)
{
	m_states[a_string] = a_state;//adds the game state a_state to the gamestate manager and designates its location as a_string
	a_state->m_parent = this;// sets the pointer in the class to point to the game state manager
}
void GameStateManager::removeGameState(std::string a_string)//removes the game state that is specified
{
	m_states[a_string] = nullptr;//replaces the gamestate with a null value
}
void GameStateManager::update()
{
	for (auto itr = m_states.begin(), itrEnd = m_states.end(); itr != itrEnd; itr++)//magic. dont quite get it myself.
	{//essentially it goes throught the gamestatemanager  until all of the the game states have been checked
		if (itr->second != nullptr && itr->second->m_isPaused != true)//checks if the spot is either a nullprt or if its paused
		{
			itr->second->update();//updates whats is active in the manager
		}
			//first is the name of it
			//second is the value
	}
}
void GameStateManager::draw()
{
	for (auto itr = m_states.begin(), itrEnd = m_states.end(); itr != itrEnd; itr++)//magic. dont quite get it myself.
	{//essentially it goes throught the gamestatemanager  until all of the the game states have been checked
		if (itr->second != nullptr && itr->second->m_isPaused != true)//checks if the spot is either a nullprt or if its paused
		{
			itr->second->draw();//updates whats is active in the manager
		}
		//first is the name of it
		//second is the value
	}
}

GameState* GameStateManager::getGameState(std::string a_state)//returns that games state
{
	if (m_states[a_state] != NULL)//checks whether the game state actually excists
	{
		return m_states[a_state];//returns the game state
	}
	else//if it doesnt excists it gives this error. prevents exceptions from being throw
	{
		return nullptr;
	}
}

//////game states

void GameState::setPaused(bool a_paused)//sets whether the game is paused or not.
{
	m_isPaused = a_paused;
}