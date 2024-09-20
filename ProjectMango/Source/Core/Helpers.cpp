#include "pch.h"
#include "Helpers.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "Graphics/Raycast.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Biome.h"


float EaseOut(float value, int easing_factor)
{
	return 1.0f - Power<float>(1.0f - value, easing_factor);
}

bool RaycastToFloor(ECS::Entity entity, float& out_distance)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	if(const ECS::Transform* transform = ecs->GetComponent(Transform, entity))
	{
		VectorF bot = transform->GetRect().BotCenter();
			
		std::vector<u32> collider_flags;
		collider_flags.push_back(ECS::Collider::IsFloor);
			
		const ECS::Level& level = ECS::Biome::GetLevel(entity);

		RaycastResult result;
		Raycast(bot, VectorF(0.0f, 1.0f), level.size.y, result, nullptr, &collider_flags);

		out_distance = result.distance;
		return result.hasHit;
	}

	return false;
}