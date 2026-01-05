#include "pch.h"
#include "Raycast.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/SpacialComponents.h"
#include "ECS/ComponentArray.h"
#include "Core/Helpers.h"

#include "Debugging/ImGui/ImGuiMainWindows.h"

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

	// assume here that nothing is narrower than 4 pixles
	const float ray_increment = 4.0f;

	while( ray_distance < distance )
	{
		// -- broad phase --
		VectorF ray_point = from + ray_direction * ray_distance;

		for( u32 i = 0; i < target_colliders.size(); i++ )
		{
			// hit something
			if(target_colliders[i]->Contains(ray_point))
			{
				// -- narrow phase -- 
				// go back 1 increment, decrease ray increments and do a more accurate test
				ray_distance -= ray_increment;
				ray_point = from + ray_direction * ray_distance;
	
				const float small_ray_increment = 1.0f;
				int count = (int)(ray_increment / small_ray_increment);

				for( u32 j = 0; j < count; j++ )
				{
					ray_distance += small_ray_increment;
					ray_point = from + ray_direction * ray_distance;

					if(target_colliders[i]->Contains(ray_point))
					{
						result.entity = target_colliders[i]->entity;

						// bump it back up to just before it colided, otherwise we're likely just inside something causing it to get stuck
						// dont do this, it creates a small gap that looks bad
						result.distance = ray_distance;// - small_ray_increment;
						result.hitPosition = ray_point;
						result.hasHit = true;

						if(DebugMenu::GetState().drawRaycasts)
							DebugDraw::Line(from, ray_point, SColour::Red);

						return;
					}

				}
			}
		}

		ray_distance += ray_increment;
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
		ASSERT(!transform->GetRect().Size().isZero(), "cant raycast to floor if the size hasnt been set");

		// shift it a little right so we dont raycast of an edge (since we're using the world pos i.e. top left)
		// using the x center would make more sense but large sprites can cause it to fall off an edge
		VectorF top = transform->worldPosition + VectorF(5.0f, 0.0);

		std::vector<ECS::Entity> self;
		self.push_back(entity);

		std::vector<u32> collider_flags;
		collider_flags.push_back(ECS::Collider::IsFloor);
			
		const ECS::Level& level = ECS::Biome::GetLevel(entity);

		RaycastResult result;
		Raycast(top, VectorF(0.0f, 1.0f), level.size.y, result, &self, &collider_flags);

		if(result.hasHit)
		{
			float top_to_bottom = transform->GetObjectRect().BotPoint() - top.y;
			out_distance = result.distance - top_to_bottom;

			//// a bit hacky here, actually snap it to the floor, otherwise it floats 1 pixel above
			//out_distance++;
		}

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
