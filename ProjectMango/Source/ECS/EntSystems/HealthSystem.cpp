#include "pch.h"
#include "HealthSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/GraphicComponents.h"

namespace ECS
{
	void HealthSystem::Init() { }

	void HealthSystem::Update(float dt)
	{
 		for (Entity entity : entities)
		{
			if (DeathScentence* ds = GetComponent(DeathScentence, entity))
			{
				if(ds->deathTimer != -FLT_MAX)
				{
					ds->deathTimer -= dt;
				}

				if(!ds->CanDie())
					return;

				bool destroy_entity = false;

				// animator trigger
				if( ds->deathLoops != -1)
				{
					bool animate_on_exit = false;

					const Animator* animator = GetComponent(Animator, entity);
					if(animator)
					{
						animate_on_exit = animator->GetAnimation(ds->action) != nullptr;
					}

					if(animate_on_exit)
					{
						if(animator->GetActiveAnimation().action == ds->action)
						{
							if(animator->loopCount >= ds->deathLoops)
							{
								destroy_entity = true;
							}
						}
					}
					else
					{
						destroy_entity = true;
					}
				}

				if(destroy_entity)
				{
					if(ds->fadeOutTime > 0.0f && ds->fadeOutTime > ds->fadeOutTimer)
					{
						ds->fadeOutTimer += dt;

						if(Sprite* sprite = GetComponent(Sprite, entity))
						{
							float progress = ds->fadeOutTimer / ds->fadeOutTime;
							float alpha = 1.0f - Maths::EaseOutCubic(progress);

							sprite->params.colourMod.setOpacity(alpha);
						}
					}
					else
					{
						ecs->entities.KillEntity(entity);
					}
				}
			}
		}
	}
}