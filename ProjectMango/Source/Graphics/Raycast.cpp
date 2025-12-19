#include "pch.h"
#include "Raycast.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Collider.h"
#include "ECS/ComponentArray.h"
#include "Core/Helpers.h"

#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "ECS/Components/Biome.h"
#include "ECS/Components/Components.h"

void Raycast(VectorF from, VectorF direction, float distance, RaycastResult& result, const std::vector<ECS::Entity>* ignored, std::vector<u32>* collider_flags)
{
	const ECS::ComponentArray<ECS::Collider>& colliders = GetAllComponents(Collider);
	const u32 count = (u32)colliders.entityToComponent.size();

	std::vector<const ECS::Collider*> target_colliders;
	
	for( auto iter = colliders.entityToComponent.begin(); iter != colliders.entityToComponent.end(); iter++ )
	{
		u32 component_index = iter->second;
		const ECS::Collider& collider = colliders.GetComponentByIndex(component_index);

		if(collider.HasFlag(ECS::Collider::IgnoreAll))
			continue;

		if(collider_flags)
		{	
			bool should_ignore = false;
			for( u32 cf = 0; cf < collider_flags->size(); cf++ )
			{
				ECS::Collider::Flags flag = (ECS::Collider::Flags)collider_flags->at(cf);
				if(!collider.HasFlag(flag))
				{
					should_ignore = true;
					break;
				}
			}
					
			if(should_ignore)
				continue;
		}

		ECS::Entity ent = collider.entity;
		if(ignored)
		{
			bool should_ignore = false;
			for( u32 ign = 0; ign < ignored->size(); ign++ )
			{
				if(ignored->at(ign) == ent)
				{
					should_ignore = true;
					break;
				}
			}

			if(should_ignore)
				continue;
		}

		// todo: do something smart here, so colliders we can ignore based on the position and direction of the ray

		target_colliders.push_back(&collider);
	}

	VectorF ray_direction = direction.normalise();

	float ray_distance = 0.0f;

	while( ray_distance < distance )
	{
		VectorF ray_point = from + ray_direction * ray_distance;

		for( u32 i = 0; i < target_colliders.size(); i++ )
		{
			if(target_colliders[i]->Contains(ray_point))
			{
				result.entity = target_colliders[i]->entity;
				result.distance = ray_distance;
				result.hitPosition = ray_point;
				result.hasHit = true;

				if(DebugMenu::GetState().drawRaycasts)
					DebugDraw::Line(from, ray_point, SColour::Red);
				return;
			}
		}

		ray_distance += 2.0f;
	}
	
	if(DebugMenu::GetState().drawRaycasts)
		DebugDraw::Line(from, from + ray_direction * distance, SColour::Green);
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
	// bump it up a little
	out_distance -= 1.0f;
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
		VectorF top = VectorF(transform->GetObjectCenter().x, rect.TopPoint());

		std::vector<ECS::Entity> self;
		self.push_back(entity);

		std::vector<u32> collider_flags;
		collider_flags.push_back(ECS::Collider::IsFloor);
			
		const ECS::Level& level = ECS::Biome::GetLevel(entity);

		RaycastResult result;
		Raycast(top, VectorF(0.0f, 1.0f), level.size.y, result, &self, &collider_flags);

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
			
		std::vector<ECS::Entity> self;
		self.push_back(entity);

		std::vector<u32> collider_flags;
		collider_flags.push_back(ECS::Collider::IsWall);
			
		const ECS::Level& level = ECS::Biome::GetLevel(entity);

		RaycastResult result;
		Raycast(bot, direction, level.size.y, result, &self, &collider_flags);

		out_distance = result.distance;
		return result.hasHit;
	}

	return false;
}
