#pragma once
#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "Input/InputManager.h"
#include "ECS/EntSystems/PathingSystem.h"
#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"
#include "ECS/Components/TileMap.h"

#include "Characters/States/EnemyStates.h"

#include "System/Window.h"

static bool s_displayPath = false;
static bool s_displayTarget = false;
static bool s_displayTargetPoints = false;

static std::pair<VectorF, Colour> s_directions[ECS::Direction::Count];


static ECS::Entity s_entWithNoPath;
bool DebugMenu::RemovePath(ECS::Entity& entity) { return s_entWithNoPath == entity; }

void DebugMenu::SetPathAttackPoint(VectorF point, Colour colour, ECS::Direction direction)
{
	s_directions[direction] = std::pair(point,colour);
}

ECS::Component::Type DebugMenu::DoPathingDebugMenu(ECS::Entity& entity)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Component::Type type = ECS::Component::Pathing;

	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::Pathing& pathing = ecs->GetComponentRef(Pathing, entity);
		InputManager* input = InputManager::Get();
		ImGui::PushID(entity + (int)type);

		ImGui::Checkbox("Display Path", &s_displayPath);
		if (s_displayPath)
		{
			const VectorF world_tile_size = ECS::TileMap::GetMapSizeRatio() * ECS::c_tileSize;
			std::vector<VectorI> path = pathing.path;

			for( u32 i = 0; i < path.size(); i++ )
			{
				VectorF pos = path[i].toFloat() * world_tile_size;
				VectorF size = world_tile_size;
				RectF rect(pos, size);

				DebugDraw::RectOutline(rect, Colour::Blue);
			}
		}

		ImGui::Checkbox("Display Target", &s_displayTarget);
		if(s_displayTarget)
		{
			const ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);

			const VectorI directions[4] = { VectorI(0,-1), VectorI( 1, 0), VectorI(0, 1), VectorI(-1, 0) };
			for( u32 i = 0; i < 4; i++ )
			{
				const float attack_range = Enemy::GetAttackRange(entity, directions[i] );
				VectorF looking_at = transform.rect.Center() + (directions[i].toFloat() * attack_range);
					
				Colour colour = Colour::Green;

				VectorI direction;
				if( ECS::Pathing* pathing = ecs->GetComponent(Pathing, entity) )
				{
					if(ecs->IsAlive(pathing->target))
					{
						if(Enemy::IsInAttackRange(entity, pathing->target, direction))
						{
							colour = Colour::Red;
						}
					}
				}

				DebugDraw::Point(looking_at, 10, colour);
			}
		}

		bool is_entity = s_entWithNoPath == entity;

		if(ImGui::Checkbox("Remove entity pathing", &is_entity))
		{
			if(is_entity)
			{
				s_entWithNoPath = entity;
			}
			else
			{
				s_entWithNoPath = ECS::EntityInvalid;
			}
		}

		ImGui::Checkbox("Target Attack Position Points", &s_displayTargetPoints);
		if(s_displayTargetPoints)
		{
			for( u32 i = 0; i < ECS::Direction::Count; i++ )
			{
				DebugDraw::Point(s_directions[i].first, 10, s_directions[i].second);
			}
		}
	
		ImGui::PopID();
	}

	return type;
}