#pragma once

#include "Game/States/State.h"

class GameState : public State
{
public:
	void Init() override;
	void Update(float dt) override;
	void FastUpdate(float dt) override;
	void HandleInput() override;
	
	void Pause() override;
	void Resume() override;
	void Exit() override;
	
	ECS::Entity activeMap;

private:
	void initCamera();
};
