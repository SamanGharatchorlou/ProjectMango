#include "pch.h"
#include "SpawnSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/IncludeComponents.h"
#include "Entities/EntityBuilder.h"
#include "Graphics/Raycast.h"
#include "Entities/CardRegistry.h"

namespace ECS
{
	void SetSpawnPosition( const Spawner& spawner, Entity entity_to_spawn )
	{
		// position it on the spawner
		RectF spawner_rect = GetRect(spawner.entity);

		// center it, but move to the top, we will raycast down to find the correct y pos
		ECS::Transform& transform = GetComponentRef(Transform, entity_to_spawn);
		transform.SetObjectCenter(spawner_rect.TopCenter());

		float distance = 0.0f;
		if( RaycastToFloor(transform.GetObjectRect(), distance) )
			transform.SetWorldPosition( transform.worldPosition + VectorF(0.0f, distance));
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

					if(const Card* card = CardRegistry::LookupCard(request.cardRegistryIndex))
					{
						// apply card damage
						if(card->points > 0)
						{
							Damage& damage = AddComponent(Damage, spawner.spawnedEntity);
							damage.value = (float)card->points;
						}
					}
					
					if(request.owner != EntityInvalid)
					{
						Faction& faction = AddComponent(Faction, spawner.spawnedEntity);
						Faction::SetAsAlliedFaction(request.owner, faction);
					}

					// update the position once we have created the object
					SetSpawnPosition(spawner, spawner.spawnedEntity);
				}

				bool finished_spawning = false;

				// fade it in during the animation
				Animator* animator = GetComponent(Animator, entity);
				if(animator && animator->GetActiveAnimation())
				{
					float alpha = (float)animator->frameIndex / (float)animator->GetActiveAnimation()->frameCount;
					if(Sprite* sprite = GetComponent(Sprite, spawner.spawnedEntity))
					{
						sprite->params.colourMod.a = (Uint8)(c_alphaMax * alpha);
					}

					// finished spawning
					if(animator->loopCount > 0)
					{			
						if(Sprite* sprite = GetComponent(Sprite, spawner.spawnedEntity))
						{
							sprite->params.colourMod.a = c_alphaMax;
						}
						
						finished_spawning = true;
					}
				}
				else
				{
					finished_spawning = true;
				}

				if(finished_spawning)
				{
					// remove the spawn request
					ecs->entities.KillEntity(spawner.spawnRequest);

					// clear up the data
					spawner.spawnedEntity = EntityInvalid;
					spawner.spawnRequest = EntityInvalid;

					// move back to idle
					state.next = Action::Inactive;
				}
			}
			else
			{
				//// wait until the collider is clear
				//if(Collider* collider = GetComponent(Collider, entity))
				//{
				//	if(collider->collisions.size() > 0)
				//		continue;
				//}

				//const Faction* spawner_faction = GetComponent(Faction, entity);
				Faction::Team spawner_faction = Faction::GetTeam(entity);

				const ComponentArray<SpawnRequest>& requests = GetAllComponents(SpawnRequest);

				Entity next_to_spawn = EntityInvalid;
				int earliest_frame = INT_MAX;

				for( auto iter = requests.entityToComponent.begin(); iter != requests.entityToComponent.end(); iter++ )
				{
					const SpawnRequest& request = requests.GetComponentByIndex(iter->second);

					bool same_faction = spawner_faction == Faction::GetTeam(request.owner);

					if(same_faction && request.frameTime < earliest_frame)
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
		}
	}
}
