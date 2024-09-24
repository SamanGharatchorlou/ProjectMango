#include "pch.h"
#include "AIController.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Physics.h"
#include "Graphics/Raycast.h"
#include "Core/Helpers.h"
#include "ECS/Components/Biome.h"

namespace ECS
{
	float AIController::DistanceToTargetSquared() const
	{
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		
		ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);

		if(target != ECS::EntityInvalid)
		{
			VectorF distance = transform.GetObjectCenter() - GetPosition(target);
			return distance.lengthSquared();
		}

		return -1.0f;
	}

	bool AIController::CanMoveForward(int ease_factor, float dt) const
	{
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
		ECS::Physics& physics = ecs->GetComponentRef(Physics, entity);
		ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);
		
		const VectorI facing_direction = state.GetFacingDirection();
		VectorF speed = physics.GetMovementEase(facing_direction.toFloat(), dt, ease_factor);

		VectorF translation = speed * dt;
		RectF rect = GetRect(entity);
		rect.Translate(translation);
		
		RaycastResult result;
		if(facing_direction.x < 0)
		{
			VectorF start = rect.LeftCenter();
			RaycastToFloor(start, result);
		}
		else if(facing_direction.x > 0)
		{
			VectorF start = rect.RightCenter();
			RaycastToFloor(start, result);
		}

		if(result.hasHit)
		{
			VectorF position = result.hitPosition;

			const ECS::Level& level = ECS::Biome::GetLevel(position);	
			VectorI index = level.GetTileIndex(position);

			int traversal_value = level.walkableTiles.get(index);
			if(traversal_value == 1)
				return true;
		}

		return false;
	}
}