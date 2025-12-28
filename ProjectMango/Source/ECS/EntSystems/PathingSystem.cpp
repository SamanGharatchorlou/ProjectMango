#include "pch.h"
#include "PathingSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/SpacialComponents.h"
#include "ECS/Components/AIComponents.h"
#include "Graphics/Raycast.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"

namespace ECS
{
	static bool CanMoveDistance(Entity entity, const ECS::Level* level, VectorF target_position)
	{
		ECS::Transform& transform = GetComponentRef(Transform, entity);

		VectorF position = GetPosition(entity);
		VectorF translation = target_position - position;
		float length = translation.length();
		if (length == 0)
			return true;

		RectF rect = GetRect(entity);
		rect.Translate(translation);

		VectorF start = translation.x < 0 ? rect.LeftCenter() : rect.RightCenter();
		VectorF direction = translation.x < 0 ? VectorF(-1.0f, 0.0f) : VectorF(1.0f, 0.0f);

		// is there anything blocking the path
		RaycastResult horizontal_result;
		Raycast(start, direction, length, horizontal_result);
		if (horizontal_result.hasHit)
			return false;

		// is the tile at the new location valid
		RaycastResult down_resut;
		RaycastToFloor(start, down_resut);

		if (down_resut.hasHit)
		{
			VectorF position = down_resut.hitPosition;
			VectorI index = level->GetTileIndex(position);

			if (index.isPositive())
			{
				int traversal_value = level->walkableTiles.get(index);
				if (traversal_value == 1)
					return true;
			}
		}

		return false;
	}


	void PathingSystem::Update(float dt)
	{
		const Level& active_level = Biome::GetVisibleLevel();
		if(active_level.walkableTiles.rows() == 0 )
			return;

 		for (Entity entity : entities)
		{
			// debug break point
			if(DebugMenu::GetSelectedEntity() == entity)
				int a = 4;

			Pathing& pathing = GetComponentRef(Pathing, entity);
			//AIController& aic = GetComponentRef(AIController, entity);



			// reset this every frame
			pathing.hasValidPath = false;

			//VectorF pos = GetPosition(entity);
			//const Level& level = Biome::GetLevel(pos);
			//levelIndex = level.index;
			//const ECS::Level* level = ECS::Biome::GetLevelFromIndex(pathing.levelIndex);
			//if (!level)
			//{
			//	DebugPrint(Warning, "Entity %s pathin level index invalid", GetName(entity));
			//	continue;
			//}

			if (!active_level.IsPointInBounds(pathing.targetLocation))
				continue;

			VectorF target = pathing.targetLocation;
			VectorF position = GetPosition(entity);

			// !!! this then fails !!!
			bool can_move_to_target_location = CanMoveDistance(entity, &active_level, pathing.targetLocation);
			if (!can_move_to_target_location)
				continue;

			// passed all the tests, can move to the next location
			pathing.hasValidPath = true;

			// move the physics
			Physics& physics = GetComponentRef(Physics, entity);
			physics.speed += (pathing.targetLocation - position);
		}
	}
}