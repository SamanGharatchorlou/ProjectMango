#include "pch.h"
#include "AIControllerSystem.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Core/Helpers.h"
#include "Entities/Enemies/AIStrategies.h"

namespace ECS
{
	// UNUSED
	void AIControllerSystem::Update(float dt)
	{
 		for (Entity entity : entities)
		{
			// debug break point
			//if(IsSelectedDebugEntity(entity))
			//	int a = 4;

			//AIController& aic = GetComponentRef(AIController, entity);

			//if(AIIntent* intent = GetComponent(AIIntent, entity))
			//{
			//	// reset intent
			//	*intent = AIIntent();

			//	if(TurnState* turn = GetComponent(TurnState, entity))
			//	{
			//		if(!turn->IsCurrentTurn())
			//			continue;
			//	}

			//	if(AIStrategy* strategy = GetComponent(AIStrategy, entity))
			//	{
			//		// here check the type etc
			//		AIStrategies::ShockSweeper(entity, *intent);
			//	}
			//	else
			//		AIStrategies::SimpleAttacker(entity, *intent);
			//}
		}
	}
}