#include "Game/States/State.h"
#include "Game/States/StateMachine.h"

//enum class SystemStates
//{
//	None,
//	StartupState,
//	PreGameState,
//	GameState,
//	PauseState,
//};

struct SystemStateManager
{
	//friend class GameController;

	SystemStateManager();

	//// System game states
	//void addState(SystemStates state);
	//void replaceState(SystemStates state);
	//void popState();

	//static State* getNewGameState(SystemStates state);

	//template<class T>
	//const T* GetActiveState() const { return static_cast<T*>(&mStates.Top()); }

	//void restart() { mRestart = true; }
	//void quit() { mQuit = true; }

	StateMachine<State> mStates;

	bool mQuit = false;
	bool mRestart = false;
};