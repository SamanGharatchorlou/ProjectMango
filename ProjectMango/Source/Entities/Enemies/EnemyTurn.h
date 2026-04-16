#pragma once

namespace ECS
{
	struct TurnState;
}

namespace EnemyTurn
{
	void Update(ECS::TurnState& turn);
	void OnEndTurn(ECS::TurnState& turn);
}