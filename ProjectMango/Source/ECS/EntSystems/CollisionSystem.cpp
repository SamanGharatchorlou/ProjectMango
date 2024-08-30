#include "pch.h"
#include "CollisionSystem.h"

#include "Core/Helpers.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "ECS/ComponentArray.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "Game/FrameRateController.h"

namespace ECS
{
	static bool CanCollide(EntityCoordinator* ecs, const Collider& A_collider, bool A_isDamage, bool A_isPhysical, const Collider& B_collider)
	{
		ECS::Entity B_entity = B_collider.entity;
		if(B_collider.HasFlag(Collider::IgnoreAll))
			return false;

		const bool B_is_damage = ecs->HasComponent(Damage, B_collider.entity);
		const bool B_is_physical =  !B_is_damage;

		// ignore checking against damage colliders
		if(B_is_damage)
		{
			return false;
		}
		else if (B_is_physical)
		{
			if (A_collider.HasFlag(Collider::IgnorePhysical))
				return false;
		}

		// if this applies damage we only need to do the check if they have health
		if(A_isDamage && !ecs->HasComponent(Health, B_entity))
			return false;

		// if physical and ignoring collisions skip
		if( A_isPhysical )
		{
			if (A_collider.HasFlag(Collider::IgnorePhysical))
				return false;

			// if the collider we're checking against is physical but is ignoring collisions then skip
			if(B_is_physical)
			{
				if(B_collider.HasFlag(Collider::IgnorePhysical))
					return false;
			}
		}

		return true;
	}

	bool CollisionSystem::DoesColliderInteract(ECS::Entity entity)
	{
		// debug break point
		if(DebugMenu::GetSelectedEntity() == entity)
			int a = 4;
		
		EntityCoordinator* ecs = GameData::Get().ecs;
		const ComponentArray<Collider>& colliders = ecs->GetComponents<Collider>(Component::Type::Collider);
		const std::vector<Collider>& collider_list = colliders.components;

		const Collider& A_collider = ecs->GetComponentRef(Collider, entity);

		// ignore static colliders, we check against them, but not from them (or if we're ignoring all)
		if(A_collider.HasFlag(Collider::Static) || A_collider.HasFlag(Collider::IgnoreAll))
			return false;

		const u32 index = colliders.GetComponentIndex(entity);
		const bool A_is_damage = ecs->HasComponent(Damage, entity);
		const bool A_is_physical =  !A_is_damage;

		for (u32 i = 0; i < collider_list.size(); i++) 
		{
			// ignore ourself
			if(i == index)
				continue;
				
			const Collider& B_collider = collider_list[i];

			if(!CanCollide(ecs, A_collider, A_is_damage, A_is_physical, B_collider))
				continue;

			if( A_collider.intersects(B_collider) )
				return true;
        }

		return false;
	}

	static const float c_colliderGap = 1.0f;

	void CollisionSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		const FrameRateController& frc = FrameRateController::Get();
		const int frame_count = frc.frameCount();

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

			// ignore static colliders, we check against them, but not from them (or if we're ignoring all)
			if (A_collider.HasFlag(Collider::Static) || A_collider.HasFlag(Collider::IgnoreAll))
				continue;

			const u32 index = colliders.GetComponentIndex(entity);
			const Damage* A_damage = ecs->GetComponent(Damage, entity);
			const bool A_is_damage = A_damage != nullptr;
			const bool A_is_physical =  !A_is_damage;

			// no point in checking anything if we're not moving
			if( A_is_physical && A_collider.allowedMovement.isZero() )
				continue;

			for (u32 i = 0; i < count; i++) 
			{
				// ignore ourself
				if(i == index)
					continue;
				
				Collider& B_collider = collider_list[i];

				if(!CanCollide(ecs, A_collider, A_is_damage, A_is_physical, B_collider))
					continue;

				if(A_collider.intersects(B_collider)) 
				{
					ECS::Entity B_entity = B_collider.entity;
					const bool did_add_a = PushBackUnique(A_collider.collisions, B_entity);
					const bool did_add_b = PushBackUnique(B_collider.collisions, entity);

					if(did_add_b && A_is_damage)
						B_collider.lastHitFrame = frame_count;

					// ghost colliders just check for collisions and have no effect so dont compute anything below
					if(A_collider.HasFlag(Collider::Flags::GhostCollider))
						continue;

					// Damage
					if( A_damage )
					{
						Health* B_health = ecs->GetComponent(Health, B_entity);
						B_health->ApplyDamage(*A_damage);
					}
					// Physical, can we slide
					else
					{
						VectorF velocity = A_collider.forward - A_collider.back;
						if (velocity.isZero())
							continue;

						// roll back both colliders, then roll this one forward axis by axis to check which way it can move
						A_collider.RollBackPosition();
						B_collider.RollBackPosition();
						 
						bool still_interacts = A_collider.intersects(B_collider);
						if (!still_interacts)
						{
							// rolled back rect
							RectF rect = A_collider.rect;

							// doesnt matter on the direction, always seems to fail NOT with static colliders though
							// only with moving colliders to be fair, maybe theres something in that
							const RectF horizontal_rect = rect.MoveCopy(VectorF(velocity.x, 0.0f));
							const bool cannot_move_horizontally = B_collider.intersects(horizontal_rect);
							if(cannot_move_horizontally)
							{
								A_collider.allowedMovement.x = 0;
							}

							const RectF vertical_rect = rect.MoveCopy(VectorF(0.0f, velocity.y));
							const bool cannot_move_vertically = B_collider.intersects(vertical_rect);
							if(cannot_move_vertically)
							{
								A_collider.allowedMovement.y = 0;

								float b_top = B_collider.rect.TopPoint();
								float a_bot = A_collider.rect.BotPoint();
								if(b_top > a_bot)
								{
									float distance = b_top - a_bot - c_colliderGap;
									if( distance > 0)
										A_collider.allowedMovement.y = distance;
								}
							}
						}
						else
						{
							// we're proably stuck here...?
							A_collider.allowedMovement = VectorF::zero();
						}

						A_collider.RollForwardPosition();
						B_collider.RollForwardPosition();
					}
				}
            }
		}
	}
}