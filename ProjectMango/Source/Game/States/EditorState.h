#pragma once
#include "Game/States/State.h"

class EditorState : public State
{
public:
	void Init() override;
	void Update(float dt) override;
	void FastUpdate(float dt) override { };
	void HandleInput() override;
	void Exit() override;

	void Resume() { };
	
	TimerF timer;

	void OpenAnimationEditor();
	void OpenUIEditor();
	//bool animationEditor = false;
	//bool UiEditor = false;
};
