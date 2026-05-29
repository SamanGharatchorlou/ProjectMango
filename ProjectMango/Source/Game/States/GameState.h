#pragma once

#include "Game/States/State.h"

// todo try remove
#include "ECS/Components/GameComponents.h"

struct EndGameState
{
	bool gameOver = false;
	bool showingGameOverText = false;
	bool showingRelicSelectScreen = false;

	bool beginNextBattle = false;
};

struct PlayerState
{
	std::vector<ECS::Relic> relics;
	float health;
};

class GameState : public State
{
public:
	void Init() override;
	void Update(float dt) override;
	void HandleInput() override;
	void Exit() override;
	
	static GameState* GetActive();
	static int GetTurnIndex();

	void NextBattle();

	EndGameState endGameState;
	PlayerState playerState;

	ECS::Entity enemy = ECS::EntityInvalid;

	int turnIndex = 0;
	bool autoConfirmTurn = true;


private:
	void initCamera();
};