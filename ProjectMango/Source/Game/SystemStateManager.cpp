#include "pch.h"
#include "SystemStateManager.h"

#include "Game/States/PreGameState.h"
#include "Game/States/GameState.h"
#include "Game/States/PauseState.h"
#include "Game/Data/GameData.h"

SystemStateManager::SystemStateManager() : mStates(new NullState) { }


// System game states
void SystemStateManager::addState(SystemStates state)
{
	mStates.addState(SystemStateManager::getNewGameState(state));
}
void SystemStateManager::replaceState(SystemStates state)
{
	mStates.replaceState(SystemStateManager::getNewGameState(state));
}
void SystemStateManager::popState()
{
	mStates.popState();
}


State* SystemStateManager::getNewGameState(SystemStates state)
{
	State* systemState = nullptr;

	switch (state)
	{
	case SystemStates::PreGameState:
		systemState = new PreGameState();
		break;
	case SystemStates::GameState:
		systemState = new GameState();
		break;
	case SystemStates::PauseState:
		systemState = new PauseState();
		break;
	case SystemStates::None:
	default:
		DebugPrint(Warning, "No valid system state with type %d", state);
		break;
	}

	return systemState;
}