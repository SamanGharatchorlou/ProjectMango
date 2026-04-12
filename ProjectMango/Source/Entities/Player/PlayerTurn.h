#pragma once

namespace ECS
{
	struct TurnState;
}

namespace PlayerTurn
{
	void Update(ECS::TurnState& turn);
	void OnEndTurn(ECS::TurnState& turn);
}