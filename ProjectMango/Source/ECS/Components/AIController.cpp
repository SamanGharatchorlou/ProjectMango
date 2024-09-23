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
			const ECS::Transform& target_transform = ecs->GetComponentRef(Transform, target);
			VectorF distance = transform.GetObjectCenter() - target_transform.GetObjectCenter();
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

		VectorF left = rect.LeftCenter();
		VectorF right = rect.RightCenter();

		RaycastResult left_result;
		RaycastResult right_result;
		RaycastToFloor(left, left_result);
		RaycastToFloor(right, right_result);

		if(left_result.hasHit)
		{
			VectorF position = left_result.hitPosition;

			const ECS::Level& level = ECS::Biome::GetLevel(entity);	
			VectorI index = level.GetTileIndex(position);

			int is_walkable = level.walkableTiles.get(index);

			int a = 4;
		}

		return true;
	}
}