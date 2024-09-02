#include "pch.h"
#include "PathingSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/AIController.h"
#include "ECS/Components/ComponentCommon.h"

namespace ECS
{
	void PathingSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		
 		for (Entity entity : entities)
		{
			Pathing& pathing = ecs->GetComponentRef(Pathing, entity);
			AIController& aic = ecs->GetComponentRef(AIController, entity);

			if(aic.target != EntityInvalid)
			{
				VectorF target = GetPosition(aic.target);

				// this indirectly sets the facing direction sicnce its derived from the sprite flip
				Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
				if (sprite.canFlip)
				{
					VectorF enemy = GetPosition(entity);

					if (target.x > enemy.x)
						sprite.flip = SDL_FLIP_NONE;
					else if (target.x < enemy.x)
						sprite.flip = SDL_FLIP_HORIZONTAL;
				}
			}
		}
	}
}