#include "pch.h"
#include "CollisionSystem.h"

#include "Core/Helpers.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "ECS/ComponentArray.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "Game/FrameRateController.h"

#include "ECS/Components/Biome.h"
#include "ECS/Components/ComponentCommon.h"


namespace ECS
{
	static const float c_colliderGap = 1.0f;

	static VectorF BumpCollider( const ECS::Collider& colliderA, const ECS::Collider& colliderB )
	{
		VectorF direction = colliderA.rect.Center() - colliderB.rect.Center();
		if(direction.isZero())
			return VectorF::zero();

		VectorF bump = direction.normalise();

		RectF bump_rect = colliderA.rect.MoveCopy(bump);
		bool bump_still_collides = colliderB.intersects(bump_rect);

		while(bump_still_collides)
		{
			bump += bump;

			bump_rect = colliderA.rect.MoveCopy(bump);
			bump_still_collides = colliderB.intersects(bump_rect);
		}

		return bump;
	}

	void CollisionSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		const FrameRateController& frc = FrameRateController::Get();
		const int frame_count = frc.FrameCount();

		ComponentArray<Collider>& colliders =  ecs->GetComponents<Collider>(Component::Type::Collider);
		std::vector<Collider>& collider_list = colliders.components;
		const u32 count = (u32)collider_list.size();

		for (Entity entity : entities)
		{
			// debug break point
			if(DebugMenu::GetSelectedEntity() == entity)
				int a = 4;

			Collider& A_collider = ecs->GetComponentRef(Collider, entity);

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

			const u32 index = colliders.GetComponentIndex(entity);
			
			Damage* A_damage = ecs->GetComponent(Damage, entity);
			bool is_damage = A_collider.HasFlag(Collider::IsDamage);

			for (u32 i = 0; i < count; i++) 
			{
				// ignore ourself
				if(i == index)
					continue;
				
				Collider& B_collider = collider_list[i];

				// if IgnorePhysical then its damage we only check from these (i.e. when its collider A, not B)
				if(B_collider.HasFlag(Collider::IgnoreAll) || B_collider.HasFlag(Collider::IsDamage) )
					continue; 

				if( A_collider.HasFlag(Collider::TerrainOnly) && !B_collider.HasFlag(Collider::IsTerrain) )
					continue;

				if( A_collider.HasFlag(Collider::PlayerOnly) && !B_collider.HasFlag(Collider::IsPlayer) )
					continue;

				if(A_collider.intersects(B_collider)) 
				{
					ECS::Entity B_entity = B_collider.entity;
					PushBackUnique(A_collider.collisions, B_entity);
					PushBackUnique(B_collider.collisions, entity);

					if( is_damage && !B_collider.HasFlag(Collider::IgnoreDamage) )
					{
						if(A_damage && A_damage->CanApplyTo(B_entity))
						{
							B_collider.lastHitFrame = frame_count;
 							A_damage->ApplyTo(B_entity);
						}
					}

					// damage and ghost colliders just check for collisions and have no effect so dont compute anything below
					if(is_damage || A_collider.HasFlag(Collider::Flags::GhostCollider) || A_collider.HasFlag(Collider::Flags::Kinematic))
						continue;

					// Physical, can we slide 
					VectorF& velocity = A_collider.allowedMovement;
					if (velocity.isZero())
						continue;

					// roll back collider, then roll it forward axis by axis to check which way it can move
					A_collider.RollBackPosition();

					// rolled back rect
					RectF rect = A_collider.rect;

					const RectF horizontal_rect = rect.MoveCopy(VectorF(velocity.x, 0.0f));
					const RectF vertical_rect = rect.MoveCopy(VectorF(0.0f, velocity.y));

					const bool still_interacts = A_collider.intersects(B_collider);
					if (!still_interacts)
					{
						const bool cannot_move_horizontally = B_collider.intersects(horizontal_rect);
						if(cannot_move_horizontally)
						{
							velocity.x = 0;

							// left/right collisions
							if(A_collider.desiredMovement.x > 0.0f)
								A_collider.collisionSide[Collider::Right] = true;
							else if(A_collider.desiredMovement.x < 0.0f)
								A_collider.collisionSide[Collider::Left] = true;
						}

						const RectF vertical_rect = rect.MoveCopy(VectorF(0.0f, velocity.y));
						const bool cannot_move_vertically = B_collider.intersects(vertical_rect);
						if(cannot_move_vertically)
						{
							velocity.y = 0;

							float b_top = B_collider.rect.TopPoint();
							float a_bot = A_collider.rect.BotPoint();
							if(b_top > a_bot)
							{
								float distance = b_top - a_bot - c_colliderGap;
								if( distance > 0)
									velocity.y = distance;
							}

							// top/bot collisions
							if( velocity.y == 0 )
							{
								if(A_collider.desiredMovement.y < 0.0f)
									A_collider.collisionSide[Collider::Top] = true;
								else if(A_collider.desiredMovement.y > 0.0f)
									A_collider.collisionSide[Collider::Bottom] = true;
							}
						}
					}
					else // we're still stuck
					{
						VectorF B_center = B_collider.rect.Center();

						float distance = VectorF::distanceSquared(rect.Center(), B_center);

						// see if we are trying to move away from the collider, allow that even if we're still colliding
						float distance_fwd_x = VectorF::distanceSquared(horizontal_rect.Center(), B_center);
						float distance_fwd_y = VectorF::distanceSquared(vertical_rect.Center(), B_center);

						if(distance_fwd_x < distance)
						{
							velocity.x = 0;
						}

						if(distance_fwd_y < distance)
						{
							velocity.y = 0;
						}

						// gross we're stiil not able to move
						if(A_collider.HasFlag(Collider::CanBump) && velocity.isZero())
						{
							//VectorF direction = rect.Center() - B_collider.rect.Center();

							//VectorF bump = direction.normalise() * c_colliderGap;

							//RectF bump_rect = rect.MoveCopy(bump);
							//bool bump_still_collides = B_collider.intersects(bump_rect);

							//while(bump_still_collides)
							//{
							//	bump += bump;

							//	bump_rect = rect.MoveCopy(bump);
							//	bump_still_collides = B_collider.intersects(bump_rect);
							//}

							
							VectorF bump = BumpCollider(A_collider, B_collider);
							RectF bump_rect = rect.MoveCopy(bump);

							// we cant bump into another collider, just check static colliders
							for (const Collider& static_collider : collider_list)
							{
								if(static_collider.entity == entity || static_collider.entity == B_entity )
									continue;

								if(!static_collider.HasFlag(Collider::Static))
									continue;

								const bool collides_against_static = static_collider.intersects(bump_rect);
								if( collides_against_static )
								{
									bump = VectorF::zero();
									break;
								}
							}

							velocity = bump;

							DebugPrint(PriorityLevel::Debug, "Bumping %f, %f", bump.x, bump.y);
						}
					}

					A_collider.RollForwardPosition();
				}
            }
		}
	}

	void CollisionSystem::FindValidPosition(ECS::Entity entity)
	{
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		ECS::Collider& collider = ecs->GetComponentRef(Collider, entity);

		std::vector<ECS::Entity> colliders;
		ecs->GetEntitiesWithComponent(Collider, colliders);

		std::vector<ECS::Entity> level_colliders;
		const ECS::Level& active_level = ECS::Biome::GetVisibleLevel();
		FilterEntitiesInLevel(active_level, colliders, level_colliders);

		for( u32 i = 0; i < colliders.size(); i++ )
		{
			if(colliders[i] == entity)
				continue;

			if(const ECS::Collider* collider_b = ecs->GetComponent(Collider, colliders[i]))
			{
				if(collider_b->intersects(collider))
				{
					VectorF bump = BumpCollider(collider, *collider_b);
					if(!bump.isZero())
					{
						ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);
						transform.SetWorldPosition(transform.worldPosition + bump);
					}
				}
			}
		}
	}
}

