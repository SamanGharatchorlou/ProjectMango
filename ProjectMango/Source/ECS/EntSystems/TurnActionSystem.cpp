#include "pch.h"
#include "TurnActionSystem.h"

#include "ECS/Components/IncludeComponents.h"
#include "Game/States/GameState.h"
#include "Entities/Player/PlayerTurn.h"
#include "Entities/Enemies/EnemyTurn.h"
#include "ECS/EntityCoordinator.h"
#include "Core/Helpers.h"

namespace ECS
{
	void TurnActionSystem::Update(float dt)
	{
		GameState* game_state = GameState::GetActive();
		if (!game_state || game_state->endGameState.gameOver)
			return;

		if(TurnState* turn = TurnState::GetActive())
		{
			Entity entity = turn->entity;

			if( turn->tryEndTurn && turn->canEndTurn )
			{
				// cooldown time to end the turn ~1second
				if( turn->endTurnCooldownSecs > 0.0f)
				{
					turn->endTurnCooldownSecs -= dt;
					return;
				}

				if(entity == Faction::GetPlayer())
					PlayerTurn::OnEndTurn(*turn);
				else
					EnemyTurn::OnEndTurn(*turn);

				return;
			}
		}


		if(TurnState* turn = TurnState::GetActive())
		{
			Entity entity = turn->entity;

			if(entity == Faction::GetPlayer())
				PlayerTurn::Update(*turn);
			else
				EnemyTurn::Update(*turn);
		}
		else
		{
			if(entities.size() > 0)
			{	
				// purposfully dont increment turn_index variable
				// the next entity should wait until next loop to do its turn, we need to finish any clean up
				game_state->turnIndex++;

				//replace isActiveTurn with this, but its still not working
				// always skipping over the approach phase, it should still hit it normall
				// check the way i end the turn, it seems weird
				ComponentArray<TurnState>& turn_states = GetAllComponents(TurnState);
				for( auto iter = turn_states.entityToComponent.begin(); iter != turn_states.entityToComponent.end(); iter++ )
				{
					TurnState& turn_state = turn_states.GetComponentByIndex(iter->second);
					turn_state.ResetState();
				}
			}
		}
	}
}