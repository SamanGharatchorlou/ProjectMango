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
	AIController::AIController() : 
		target(EntityInvalid), 
		canMoveToTarget(false),
		attackCooldownTime(1.0f) 
	{ }

	VectorF AIController::VectorToTarget() const
	{
		if(target != ECS::EntityInvalid)
		{
			return GetPosition(entity) - GetPosition(target);
		}

		return VectorF();
	}

	bool AIController::CanMoveForward(int ease_factor, float dt) const
	{
		ECS::Physics& physics = GetComponentRef(Physics, entity);

		const SDL_RendererFlip flip_direction = GetFacingDirection(entity);
		const VectorI facing_direction = FacingDirectionToVector(flip_direction);
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