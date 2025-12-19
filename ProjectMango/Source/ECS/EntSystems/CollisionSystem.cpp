#include "pch.h"
#include "CollisionSystem.h"

#include "Core/Helpers.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "ECS/ComponentArray.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "Game/FrameRateController.h"

namespace ECS
{
	static const float c_colliderGap = 1.0f;

	static VectorF BumpCollider( const ECS::Collider& colliderA, const ECS::Collider& colliderB )
	{
		VectorF direction = colliderA.rect.Center() - colliderB.rect.Center();
		if(direction.isZero())
			return VectorF::zero();

		VectorF bump = direction.normalise();
		VectorF bump_accum = direction.normalise();

		RectF bump_rect = colliderA.rect.MoveCopy(bump);
		bool bump_still_collides = colliderB.Intersects(bump_rect);

		int counter = 0;
		while(bump_still_collides)
		{
			if (counter > 50)
			{
				return VectorF();
			}

			counter++;
			bump_accum += bump;

			bump_rect = colliderA.rect.MoveCopy(bump_accum);
			bump_still_collides = colliderB.Intersects(bump_rect);
		}

		return bump_accum;
	}

	static void DebugTest(const Collider& collider)
	{
		// debugging
		
		const char* name = ECS::GetName(collider.entity);

		if (collider.HasFlag(ECS::Collider::Static))
			int a = 4;
		if (collider.HasFlag(ECS::Collider::IsPlayer))
			int a = 4;
		if (collider.HasFlag(ECS::Collider::IsEnemy))
			int a = 4;
		if (collider.HasFlag(ECS::Collider::IsTerrain))
			int a = 4;
		if (collider.HasFlag(ECS::Collider::IsFloor))
			int a = 4;
		if (collider.HasFlag(ECS::Collider::IsWall))
			int a = 4;
		if (collider.HasFlag(ECS::Collider::IsDamage))
			int a = 4;
		if (collider.HasFlag(ECS::Collider::IgnoreAll))
			int a = 4;
		if (collider.HasFlag(ECS::Collider::IgnoreDamage))
			int a = 4;
		if (collider.HasFlag(ECS::Collider::TerrainOnly))
			int a = 4;
		if (collider.HasFlag(ECS::Collider::GhostCollider))
			int a = 4;
		if (collider.HasFlag(ECS::Collider::CanBump))
			int a = 4;
		if (collider.HasFlag(ECS::Collider::QuadCollider))
			int a = 4;
	}

	void CollisionSystem::Update(float dt)
	{
		const FrameRateController& frc = FrameRateController::Get();
		const int frame_count = frc.FrameCount();

		ComponentArray<Collider>& colliders =  GetAllComponents(Collider);

		for (u32 i = 0; i < entitiesToDestroy.size(); i++)
		{
			ecs->entities.KillEntity(entitiesToDestroy[i]);
		}

		for (Entity entity : entities)
		{
			// debug break point
			if(DebugMenu::GetSelectedEntity() == entity)
				int a = 4;

			Collider& collider = GetComponentRef(Collider, entity);
			Transform& transform = GetComponentRef(Transform, entity);

			collider.back = transform.worldPosition;
			collider.forward = transform.targetWorldPosition;
			collider.RollForwardPosition();
		}

		for (Entity entity : entities)
		{
			// debug break point
			if(DebugMenu::GetSelectedEntity() == entity)
				int a = 4;

			Collider& A_collider = GetComponentRef(Collider, entity);

			const char* debug_collider_a_name = ECS::GetName(entity);

			for( u32 i = 0; i < A_collider.collisions.size(); i++ )
			{
				// remove one dead entry at a time, we reorder and change the size of this list 
				// so this just keeps things simple and it doesnt matter if things hang around for a bit
				if(!ecs->IsAlive(A_collider.collisions[i]))
				{
					EraseSwap(A_collider.collisions, A_collider.collisions[i]);
					break;
				}
			}

			A_collider.allowedMovement = A_collider.forward - A_collider.back;
			A_collider.desiredMovement = A_collider.allowedMovement;
			
			// debug break point
			if(DebugMenu::GetSelectedEntity() == entity && !A_collider.allowedMovement.isZero())
				int a = 4;

			memset(A_collider.collisionSide, false, sizeof(bool) * 4);

			// ignore static colliders, we check against them, but not from them (or if we're ignoring all)
			if (A_collider.HasFlag(Collider::Static) || A_collider.HasFlag(Collider::IgnoreAll))
				continue;

			Damage* A_damage = GetComponent(Damage, entity);

			bool flip_x = false;
			bool flip_y = false;

			// debugging
			DebugTest(A_collider);

			// to make sure we roll forward and back correctly, for sanity
			int debug_counter = 0;

			for( auto iter = colliders.entityToComponent.begin(); iter != colliders.entityToComponent.end(); iter++ )
			{
				ASSERT(debug_counter == 0, "Not rolling forward and back correctly! early exit where there shouldnt be?");

				Collider& B_collider = colliders.GetComponentByIndex(iter->second);
				const char* debug_collider_b_name = ECS::GetName(B_collider.entity);
				if(B_collider.entity == entity)
					continue;

				// debugging
				DebugTest(B_collider);

				// if IgnorePhysical then its damage we only check from these (i.e. when its collider A, not B)
				if(B_collider.HasFlag(Collider::IgnoreAll) || B_collider.HasFlag(Collider::IsDamage) )
					continue; 

				// ignore terrain
				if( A_collider.HasFlag(Collider::TerrainOnly) && !B_collider.HasFlag(Collider::IsTerrain) )
					continue;

				// player only
				if( A_collider.HasFlag(Collider::PlayerOnly) && !B_collider.HasFlag(Collider::IsPlayer) )
					continue;
				if( !A_collider.HasFlag(Collider::IsPlayer) && B_collider.HasFlag(Collider::PlayerOnly) )
					continue;

				if( A_collider.HasFlag(Collider::IgnorePlayer) && B_collider.HasFlag(Collider::IsPlayer) )
					continue;

				if(A_collider.Intersects(B_collider.rect)) 
				{
					ECS::Entity B_entity = B_collider.entity;
					PushBackUnique(A_collider.collisions, B_entity);
					PushBackUnique(B_collider.collisions, entity);
					const char* debug_collider_b_name_2 = ECS::GetName(B_entity);

					if( !B_collider.HasFlag(Collider::IgnoreDamage) )
					{
						if(A_damage && A_damage->CanApplyTo(B_entity))
						{
							B_collider.lastHitFrame = frame_count;
 							A_damage->ApplyTo(B_entity);
						}
					}

					bool debug_test = A_collider.Intersects(B_collider.rect);

					// destroy on contact
					if (A_collider.destroyOnContact)
					{
						entitiesToDestroy.push_back(A_collider.entity);
					}

					// damage and ghost colliders just check for collisions and have no effect so dont compute anything below
					if(A_collider.HasFlag(Collider::Flags::GhostCollider) || A_collider.HasFlag(Collider::Flags::Kinematic))
						continue;

					// Physical, can we slide 
					VectorF& allowed_movement = A_collider.allowedMovement;
					if (allowed_movement.isZero())
						continue;

					// roll back collider, then roll it forward axis by axis to check which way it can move
					A_collider.RollBackPosition();
					debug_counter++;

					// rolled back rect
					RectF rect = A_collider.rect;
					const RectF horizontal_rect = rect.MoveCopy(VectorF(allowed_movement.x, 0.0f));
					const RectF vertical_rect = rect.MoveCopy(VectorF(0.0f, allowed_movement.y));

					// do we still collide with the rolled back collider, if so its bad and we're stuck
					const bool still_interacts = A_collider.Intersects(B_collider.rect);
					if (!still_interacts)
					{
						// figure out which axis we can still move in, so we can slide
						const bool cannot_move_horizontally = B_collider.Intersects(horizontal_rect);
						if(cannot_move_horizontally)
						{
							allowed_movement.x = 0;

							// left/right collisions
							if(A_collider.desiredMovement.x > 0.0f)
								A_collider.collisionSide[Collider::Right] = true;
							else if(A_collider.desiredMovement.x < 0.0f)
								A_collider.collisionSide[Collider::Left] = true;
						}

						const bool cannot_move_vertically = B_collider.Intersects(vertical_rect);
						if(cannot_move_vertically)
						{
							allowed_movement.y = 0;

							float b_top = B_collider.rect.TopPoint();
							float a_bot = A_collider.rect.BotPoint();
							if(b_top > a_bot)
							{
								float distance = b_top - a_bot - c_colliderGap;
								if( distance > 0)
									allowed_movement.y = distance;
							}

							// top/bot collisions
							if(allowed_movement.y == 0 )
							{
								if(A_collider.desiredMovement.y < 0.0f)
									A_collider.collisionSide[Collider::Top] = true;
								else if(A_collider.desiredMovement.y > 0.0f)
									A_collider.collisionSide[Collider::Bottom] = true;
							}
						}

						if (A_collider.reboundCount > 0)
						{
							flip_y = A_collider.collisionSide[Collider::Top]  || A_collider.collisionSide[Collider::Bottom];
							flip_x = A_collider.collisionSide[Collider::Left] || A_collider.collisionSide[Collider::Right];
						}
					}
					else // we're still stuck
					{
						// Allowing moving away from the collider
						VectorF B_center = B_collider.rect.Center();

						// see if we are trying to move away from the collider, allow that even if we're still colliding
						float distance_fwd_x = VectorF::distanceSquared(horizontal_rect.Center(), B_center);
						float distance_fwd_y = VectorF::distanceSquared(vertical_rect.Center(), B_center);
						float distance = VectorF::distanceSquared(rect.Center(), B_center);
						if(distance_fwd_x < distance)
						{
							allowed_movement.x = 0;
						}
						if(distance_fwd_y < distance)
						{
							allowed_movement.y = 0;
						}

						if (Physics* A_physics = GetComponent(Physics, entity))
						{
 							if (A_physics->onFloor)
							{
								if (B_collider.HasFlag(Collider::IsFloor))
								{
									// likely positive since we're inside it
									float distance_to_floor = A_collider.rect.BotPoint() - B_collider.rect.TopPoint();
									if (distance_to_floor > 0)
									{
										allowed_movement.y = -(distance_to_floor + c_colliderGap);
									}
								}
							}
						}

						// gross we're stiil not able to move
						if(A_collider.HasFlag(Collider::CanBump) && allowed_movement.isZero())
						{	
							VectorF bump = BumpCollider(A_collider, B_collider);
							allowed_movement = bump;

							DebugPrint(PriorityLevel::Debug, "Bumping %f, %f", bump.x, bump.y);
						}
					}

					A_collider.RollForwardPosition();
					debug_counter--;
				}
            }

			// only want to flip the direction once per loop, otherwise a double contact can double flip
			if(flip_x || flip_y)
			{
				if (Physics* A_physics = GetComponent(Physics, entity))
				{
					if (flip_x)
						A_physics->speed.x = A_physics->speed.x * -1;

					if (flip_y)
						A_physics->speed.y = A_physics->speed.y * -1;

					// Rotate sprite
					if (ECS::Sprite* sprite = GetComponent(Sprite, entity))
					{
						sprite->rotation = A_physics->speed.getRotation();
					}
				}

				A_collider.reboundCount--;
				if(A_collider.reboundCount == 0)
				{
					A_collider.destroyOnContact = true;
				}
			}
		}
	}

	void CollisionSystem::FindValidPosition(Entity entity)
	{
		
		Collider& A_collider = GetComponentRef(Collider, entity);

		ComponentArray<Collider>& colliders =  GetAllComponents(Collider);
		
		for( auto iter = colliders.entityToComponent.begin(); iter != colliders.entityToComponent.end(); iter++ )
		{
			Collider& B_collider = colliders.GetComponentByIndex(iter->second);

			if(B_collider.entity == entity)
				continue;

			if(A_collider.Intersects(B_collider.rect))
			{
				if (Physics* A_physics = GetComponent(Physics, entity))
				{
					if (A_physics->onFloor)
					{
						if (B_collider.HasFlag(Collider::IsFloor))
						{
							// likely positive since we're inside it
							float distance_to_floor = A_collider.rect.BotPoint() - B_collider.rect.TopPoint();
							if (distance_to_floor > 0)
							{
								ECS::Transform& transform = GetComponentRef(Transform, entity);
								transform.SetWorldPosition( transform.worldPosition - (distance_to_floor + c_colliderGap) );
							}
						}
					}
				}
			}
		}
	}
}

