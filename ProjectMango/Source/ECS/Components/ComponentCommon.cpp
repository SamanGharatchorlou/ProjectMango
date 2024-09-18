#include "pch.h"
#include "ComponentCommon.h"

#include "ECS/EntityCoordinator.h"
#include "Components.h"
#include "ECS/Components/Collider.h"

namespace ECS
{
	//// but this might be unreliable tbh
	//// collider is probably the most accurate thing to represent its position
	//VectorF GetPosition(Entity entity)
	//{
	//	EntityCoordinator* ecs = GameData::Get().ecs;
	//	if(const Collider* collider = ecs->GetComponent(Collider, entity))
	//	{
	//		return collider->rect.TopLeft();
	//	}
	//	else if(const Transform* transform = ecs->GetComponent(Transform, entity))
	//	{
	//		return transform->worldPosition;
	//	}

	//	return VectorF::zero();
	//}
}