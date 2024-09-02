#include "pch.h"
#include "AIController.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"

namespace ECS
{
	float AIController::DistanceToTarget() const
	{
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		
		ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);

		if(target != ECS::EntityInvalid)
		{
			const ECS::Transform& target_transform = ecs->GetComponentRef(Transform, target);
			return std::abs( transform.GetCharacterCenter().x - target_transform.GetCharacterCenter().x );
		}

		return -1.0f;
	}
}