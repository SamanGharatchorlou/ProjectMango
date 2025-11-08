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


bool RaycastToFloor(const RectF& rect, float& out_distance)
{
	ASSERT(!rect.Size().isZero(), "cant raycast to floor if the size hasnt been set");

	// raycast from the top down, in case we're already in the floor
	VectorF top = rect.TopCenter();
			
	std::vector<u32> collider_flags;
	collider_flags.push_back(ECS::Collider::IsFloor);
			
	const ECS::Level& level = ECS::Biome::GetLevel(top);

	RaycastResult result;
	Raycast(top, VectorF(0.0f, 1.0f), level.size.y, result, nullptr, &collider_flags);

	out_distance = result.distance - rect.Height();
	return result.hasHit;
}

bool RaycastToFloor(const VectorF& start, RaycastResult& result)
{
	std::vector<u32> collider_flags;
	collider_flags.push_back(ECS::Collider::IsFloor);
			
	const ECS::Level& level = ECS::Biome::GetLevel(start);

	Raycast(start, VectorF(0.0f, 1.0f), level.size.y, result, nullptr, &collider_flags);
	return result.hasHit;
}

bool RaycastToFloor(ECS::Entity entity, float& out_distance)
{
	if(const ECS::Transform* transform = GetComponent(Transform, entity))
	{
		RectF rect = transform->GetRect();
		ASSERT(!rect.Size().isZero(), "cant raycast to floor if the size hasnt been set");

		// raycast from the top down, in case we're already in the floor
		VectorF top = rect.TopCenter();
			
		std::vector<u32> collider_flags;
		collider_flags.push_back(ECS::Collider::IsFloor);
			
		const ECS::Level& level = ECS::Biome::GetLevel(entity);

		RaycastResult result;
		Raycast(top, VectorF(0.0f, 1.0f), level.size.y, result, nullptr, &collider_flags);

		out_distance = result.distance - rect.Height();
		return result.hasHit;
	}

	return false;
}

bool RaycastToWall(ECS::Entity entity, VectorF direction, float& out_distance)
{
	if(const ECS::Transform* transform = GetComponent(Transform, entity))
	{
		VectorF bot = transform->GetRect().BotCenter();
			
		std::vector<u32> collider_flags;
		collider_flags.push_back(ECS::Collider::IsWall);
			
		const ECS::Level& level = ECS::Biome::GetLevel(entity);

		RaycastResult result;
		Raycast(bot, direction, level.size.y, result, nullptr, &collider_flags);

		out_distance = result.distance;
		return result.hasHit;
	}

	return false;
}



bool IsPlayer(ECS::Entity entity)
{
	if(ECS::Collider* collder = GetComponent(Collider, entity))
		return collder->HasFlag(ECS::Collider::Flags::IsPlayer);

	return false;
}
bool IsEnemy(ECS::Entity entity)
{
	if(ECS::Collider* collder = GetComponent(Collider, entity))
		return collder->HasFlag(ECS::Collider::Flags::IsEnemy);

	return false;
}
bool IsTerrain(ECS::Entity entity)
{
	if(ECS::Collider* collder = GetComponent(Collider, entity))
		return collder->HasFlag(ECS::Collider::Flags::IsTerrain);

	return false;
}


bool IsTargetInFrontOfSource(ECS::Entity target, ECS::Entity source)
{
	VectorF source_pos = ECS::GetPosition(source);
	VectorI facing = ECS::GetFacingDirectionVector(source);

	VectorF target_pos = ECS::GetPosition(target);
	// target pos relative to source
	target_pos = target_pos - source_pos;

	if(facing.x > 0)
	{
		return target_pos.x > 0;
	}
	else
	{
		return target_pos.x < 0;
	}
}