#pragma once

// class to be inherited by all game state objects
struct State
{
	virtual ~State() { }

	// pure virtual functions - require override
	virtual void Init() { } ;
	virtual void HandleInput() { };

	// State changes etc.
	virtual void Update(float dt) { };

	// Movement, collisions etc 
	virtual void FastUpdate(float dt) { };

	virtual void Exit() { };

	// optional functions
	virtual void Pause() {  }
	virtual void Resume() {  }
};


// The null state does nothing
struct NullState : public State
{
	void Init() override {}
	void HandleInput() override {}
	void Update(float /*dt*/) override { /* do nothing */ }
	void FastUpdate(float /*dt*/) override { /* do nothing */ }
	void Exit() override { 
		DebugPrint(Warning, "Removing the null state"); 
	}
};