#pragma once

#include "Game/States/State.h"

class PauseState : public State
{
public:
	void Init() override;
	void Update(float dt) override;
	void FastUpdate(float dt) override { };
	void HandleInput() override;

	void Exit() override;
};