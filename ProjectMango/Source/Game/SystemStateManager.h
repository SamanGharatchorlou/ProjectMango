#include "Game/States/State.h"
#include "Game/States/StateMachine.h"

struct SystemStateManager
{
	SystemStateManager() : mStates(new NullState) { }

	StateMachine<State> mStates;

	bool mQuit = false;
	bool mRestart = false;
};