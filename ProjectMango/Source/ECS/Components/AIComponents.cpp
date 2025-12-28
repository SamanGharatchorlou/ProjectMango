#include "pch.h"
#include "AIComponents.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/SpacialComponents.h"

namespace ECS
{
	bool AIController::CanMoveForward(int ease_factor, float dt) const
	{
		return true;
		//ECS::Physics& physics = GetComponentRef(Physics, entity);

		//const SDL_RendererFlip flip_direction = GetFacingDirection(entity);
		//const VectorI facing_direction = FacingDirectionToVector(flip_direction);
		//VectorF speed = physics.GetMovementEase(facing_direction.toFloat(), dt, ease_factor);

		//VectorF translation = speed * dt;
		//RectF rect = GetRect(entity);
		//rect.Translate(translation);
		//
		//RaycastResult result;
		//if(facing_direction.x < 0)
		//{
		//	VectorF start = rect.LeftCenter();
		//	RaycastToFloor(start, result);
		//}
		//else if(facing_direction.x > 0)
		//{
		//	VectorF start = rect.RightCenter();
		//	RaycastToFloor(start, result);
		//}

		//if(result.hasHit)
		//{
		//	VectorF position = result.hitPosition;

		//	const ECS::Level& level = ECS::Biome::GetLevel(position);	
		//	VectorI index = level.GetTileIndex(position);

		//	int traversal_value = level.walkableTiles.get(index);
		//	if(traversal_value == 1)
		//		return true;
		//}

		//return false;
	}

	

	// Pathing
	// ------------------------------------------------------------------
	Pathing::Pathing() /*: target(EntityInvalid)*/ { }

	//void Pathing::Init()
	//{

	//}

	
	void BehaviourState::Init()
	{
		if(const Config* config = GetConfigFromEntity(entity))
		{
			accelleration = config->data.GetFloat("acceleration");
			attackCooldownTimeMS = (u64)config->data.GetInt("attack_cooldown_time_ms");
			//actionCooldownTimeMS = (u64)config->data.GetInt("action_cooldown_time_ms");
		}
	}
}