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
	if(distance <= ray_increment)
		DebugPrintOnce(Warning, "Passing in a ray increment of %f, this is <=%f, it may as well be 0", distance, ray_increment);

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

bool RaycastToFloor(ECS::Entity entity, RaycastResult& out_result)
{
	if(const ECS::Transform* transform = GetComponent(Transform, entity))
	{
		ASSERT(!transform->GetRect().Size().isZero(), "cant raycast if the size hasnt been set");

		// test 3 points, left, mid, right - from the top in case we're already in the floor and need to shift up
		RectF object_rect = transform->GetRect();
		VectorF test_points[3] { object_rect.TopLeft(), object_rect.TopCenter(), object_rect.TopRight() };
		float top_to_bottom = object_rect.Size().y;

		std::vector<ECS::Entity> self;
		self.push_back(entity);

		std::vector<u32> collider_flags;
		collider_flags.push_back(ECS::Collider::IsFloor);
			
		const ECS::Level& level = ECS::Biome::GetLevel(entity);

		for( u32 i = 0; i < 3; i++ )
		{
			RaycastResult result;
			Raycast(test_points[i], VectorF(0.0f, 1.0f), level.size.y, result, &self, &collider_flags);

			if(result.hasHit)
			{
				// we're testing the top point of the object
				// so once we get the distance we need to shift it back down to the bottom
				result.distance = result.distance - top_to_bottom;
				if(result.distance < out_result.distance)
				{
					out_result = result;
				}
			}
		}

		return out_result.hasHit;
	}

	return false;
}

bool RaycastToWall(ECS::Entity entity, VectorF direction, RaycastResult& out_result)
{
	//if(const ECS::Transform* transform = GetComponent(Transform, entity))
	//{
	//	VectorF bot = transform->GetRect().BotCenter();
	//		
	//	std::vector<ECS::Entity> self;
	//	self.push_back(entity);

	//	std::vector<u32> collider_flags;
	//	collider_flags.push_back(ECS::Collider::IsWall);
	//		
	//	const ECS::Level& level = ECS::Biome::GetLevel(entity);

	//	RaycastResult result;
	//	Raycast(bot, direction, level.size.y, result, &self, &collider_flags);

	//	out_distance = result.distance;
	//	return result.hasHit;
	//}


	if(const ECS::Transform* transform = GetComponent(Transform, entity))
	{
		ASSERT(!transform->GetRect().Size().isZero(), "cant raycast if the size hasnt been set");

		// test 3 points, left, mid, right - from the center in case we're already in the wall and need to shift away
		RectF object_rect = transform->GetObjectRect();
		VectorF test_points[3] { object_rect.TopCenter(), object_rect.Center(), object_rect.BotCenter() };
		float center_shift = object_rect.Size().x * 0.5f;

		std::vector<ECS::Entity> self;
		self.push_back(entity);

		std::vector<u32> collider_flags;
		collider_flags.push_back(ECS::Collider::IsWall);
			
		const ECS::Level& level = ECS::Biome::GetLevel(entity);

		for( u32 i = 0; i < 3; i++ )
		{
			RaycastResult result;
			Raycast(test_points[i], direction, level.size.y, result, &self, &collider_flags);

			if(result.hasHit)
			{
				// we're testing the top point of the object
				// so once we get the distance we need to shift it back down to the bottom
				result.distance = result.distance - center_shift;
				if(result.distance < out_result.distance)
				{
					out_result = result;
				}
			}
		}

		return out_result.hasHit;
	}

	return false;
}
