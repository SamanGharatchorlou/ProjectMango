#include "pch.h"
#include "Raycast.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Collider.h"
#include "ECS/ComponentArray.h"
#include "Core/Helpers.h"

#include "Debugging/ImGui/ImGuiMainWindows.h"

void Raycast(VectorF from, VectorF direction, float distance, RaycastResult& result, const std::vector<ECS::Entity>* ignored, std::vector<u32>* collider_flags)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	const ECS::ComponentArray<ECS::Collider>& colliders = ecs->GetComponents<ECS::Collider>(ECS::Component::Type::Collider);
	const std::vector<ECS::Collider>& collider_list = colliders.components;

	std::vector<const ECS::Collider*> target_colliders;
	for( u32 i = 0; i < collider_list.size(); i++ )
	{
		const ECS::Collider& collider = collider_list[i];

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
			if(target_colliders[i]->contains(ray_point))
			{
				result.entity = target_colliders[i]->entity;
				result.distance = ray_distance;
				result.hitPosition = ray_point;

				if(DebugMenu::DrawRaycasts())
					DebugDraw::Line(from, ray_point, Colour::Red);
				return;
			}
		}

		ray_distance += 1.0f;
	}
	
	if(DebugMenu::DrawRaycasts())
		DebugDraw::Line(from, from + ray_direction * distance, Colour::Green);
}