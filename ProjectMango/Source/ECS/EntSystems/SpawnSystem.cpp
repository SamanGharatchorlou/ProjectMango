#include "pch.h"
#include "SpawnSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Animator.h"
#include "Entities/EntityBuilder.h"
#include "Graphics/Raycast.h"

namespace ECS
{
	void SetSpawnPosition( const Spawner& spawner, Entity entity_to_spawn )
	{
		// position it on the spawner
		VectorF spawner_center = GetPosition(spawner.entity);
		RectF spawner_rect = GetRect(spawner.entity);
				 
		// ray cast this rect onto the floor
		//RectF rect = GetRect(entity_to_spawn);

		ECS::Transform& transform = GetComponentRef(Transform, entity_to_spawn);

		// center it, but move to the top, we will raycast down to find the correct y pos
		transform.SetObjectCenter(spawner_rect.TopCenter());

		RectF rect = transform.GetObjectRect();
		//VectorF translation = spawner_center - transform.GetObjectCenter();
		//rect.Translate(translation);

		//float shift_y = 0.0f;
		float distance = 0.0f;
		if( RaycastToFloor(rect, distance) )
			transform.SetWorldPosition( transform.worldPosition + VectorF(0.0f, distance));

		//transform.SetWorldPosition( transform.worldPosition + VectorF(0.0f, distance))

		//rect.Translate(VectorF(0.0f, shift_y));
		//return rect;
	}

	void SpawnSystem::Update(float dt)
	{
		// this will only use the first spawner... probably fine for now
 		for (Entity entity : entities)
		{
			Spawner& spawner = GetComponentRef(Spawner, entity);
			EntityState& state = GetComponentRef(EntityState, entity);

			bool is_spawning = state.current == Action::Active;

			if(is_spawning)
			{
				// spawn the thing in
				if(spawner.spawnedEntity == EntityInvalid )
				{
					const SpawnRequest& request = GetComponentRef(SpawnRequest, spawner.spawnRequest);

					spawner.spawnedEntity = CreateMonster(request.emd);

					// update the position once we have created the object
					SetSpawnPosition(spawner, spawner.spawnedEntity);
					//SetWorldPosition(spawner.spawnedEntity, spawn_rect.TopLeft());
				}

				bool finished_spawning = false;

				// fade it in during the animation
				if(Animator* animator = GetComponent(Animator, entity))
				{
					const Animation& animation = animator->GetActiveAnimation();
					float alpha = (float)animator->frameIndex / (float)animation.frameCount;
					if(Sprite* sprite = GetComponent(Sprite, spawner.spawnedEntity))
					{
						sprite->colourMod.a = (Uint8)(c_alphaMax * alpha);
					}

					// finished spawning
					if(animator->loopCount > 0)
					{			
						if(Sprite* sprite = GetComponent(Sprite, spawner.spawnedEntity))
						{
							sprite->colourMod.a = c_alphaMax;
						}
						
						finished_spawning = true;
					}
				}
				else
				{
					finished_spawning= true;
				}

				if(finished_spawning)
				{
					// remove the spawn request
					ecs->entities.KillEntity(spawner.spawnRequest);

					// clear up the data
					spawner.spawnedEntity = EntityInvalid;
					spawner.spawnRequest = EntityInvalid;

					// move back to idle
					state.next = Action::Idle;
				}
			}
			else
			{
				const ComponentArray<SpawnRequest>& requests = GetAllComponents(SpawnRequest);

				Entity next_to_spawn = EntityInvalid;
				int earliest_frame = INT_MAX;

				for( auto iter = requests.entityToComponent.begin(); iter != requests.entityToComponent.end(); iter++ )
				{
					const SpawnRequest& request = requests.GetComponentByIndex(iter->second);
					if(request.frameTime < earliest_frame)
					{
						next_to_spawn = iter->first;
						earliest_frame = request.frameTime;
					}
				}

				if(next_to_spawn != EntityInvalid)
				{
					spawner.spawnRequest = next_to_spawn;
					state.next = Action::Active;
				}
			}

			// todo: handle multiple spawners
			break;
		}
	}
}
