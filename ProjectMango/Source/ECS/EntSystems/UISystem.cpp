#include "pch.h"
#include "UISystem.h"

#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "ECS/Components/UIComponents.h"
#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "Input/Cursor.h"
#include "Game/Camera/Camera.h"
#include "ECS/Components/Biome.h"
#include "System/Window.h"

namespace ECS
{
	void UISystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		std::vector<Entity> out_of_bounds_entities;

		for (Entity entity : entities)
		{
			UICursor* ui_cursor = ecs->GetComponent(UICursor, entity);
			if (ui_cursor && ui_cursor->cursor)
			{
				Camera* camera = Camera::Get();
				VectorF camera_offset = camera->GetRect().TopLeft();

				// handle window scaling
				int width = -1;
				int height = -1;
				SDL_GetWindowSize(GameData::Get().window->get(), &width, &height);
				const VectorF real_window_size = VectorF((float)width, (float)height);
				const VectorF fake_window_size = GameData::Get().window->size();
				const float render_scale = real_window_size.x / fake_window_size.x;

				Transform& transform = ecs->GetComponentRef(Transform, entity);
				VectorF map_position = (ui_cursor->cursor->position() / render_scale) + camera_offset;

				// check out of bounds
				const Biome& biome = Biome::GetActiveBiome();
				const VectorF position = transform.GetObjectCenter();
				if (position.x < biome.aabb[0].x || position.y < biome.aabb[0].y || 
					position.x > biome.aabb[1].x || position.y > biome.aabb[1].y)
				{
					out_of_bounds_entities.push_back(entity);
				}

				transform.SetWorldPosition(map_position);
				transform.size = VectorF(50, 50);
			}
		}
	}
}