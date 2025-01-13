#include "pch.h"
#include "UISystem.h"

#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"

namespace ECS
{
	void UISystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		std::vector<Entity> out_of_bounds_entities;

		for (Entity entity : entities)
		{

		}
	}
}