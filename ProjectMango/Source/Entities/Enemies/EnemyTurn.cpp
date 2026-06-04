#include "pch.h"
#include "EnemyTurn.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"

#include "Entities/Enemies/AIStrategies.h"

using namespace ECS;

void EnemyTurn::Update(ECS::TurnState& turn)
{
	Entity entity = turn.entity;
					
	if(AIIntent* intent = GetComponent(AIIntent, entity))
	{
		// reset intent
		*intent = AIIntent();

		if(AIStrategy* strategy = GetComponent(AIStrategy, entity))
		{
			// here check the type etc
			AIStrategies::ShockSweeper(entity, *intent);
		}
		else
			AIStrategies::SimpleAttacker(entity, *intent);
	}


	turn.canEndTurn = true;

	if(turn.tryEndTurn)
		turn.endTurnCooldownSecs = 1.0f;
}

void EnemyTurn::OnEndTurn(ECS::TurnState& turn)
{
	turn.turnIndex++;
	turn.ResetState();
	
	Entity entity = turn.entity;
	if(AIIntent* intent = GetComponent(AIIntent, entity))
	{
		// reset intent on ending turn - so it waits
		*intent = AIIntent();
	}
}