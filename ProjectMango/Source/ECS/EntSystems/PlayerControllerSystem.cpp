#include "pch.h"
#include "PlayerControllerSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/IncludeComponents.h"


namespace ECS
{
	void PlayerControllerSystem::Update(float dt)
	{
		for (Entity entity : entities)
		{
			PlayerController& pc = GetComponentRef(PlayerController, entity);
			
			if(AIIntent* intent = GetComponent(AIIntent, entity))
			{
				// reset intent
				*intent = AIIntent();

				if(TurnState* turn = GetComponent(TurnState, entity))
				{
					intent->wantsToBeInactive = !turn->isActiveTurn;
				}
			}
		}
	} 
}

