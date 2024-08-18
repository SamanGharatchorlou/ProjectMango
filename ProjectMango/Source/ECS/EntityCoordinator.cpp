#include "pch.h"
#include "EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"

namespace ECS
{
	void EntityCoordinator::UpdateSystems(float dt)
	{
		bool game_state_Active = false;
		if(DebugMenu::GetGamePlayerState().isActive)
		{
			game_state_Active = !DebugMenu::GetGamePlayerState().nextFrame;
			DebugMenu::GetGamePlayerState().nextFrame = false;
			dt = 1.0f / 60.0f;
		}

		for (size_t i = 0; i < systems.entSystems.size(); i++)
		{
			if(game_state_Active)
			{
				if(i != 1 && i != 4 && i != 5)
					continue;
			}

			systems.entSystems[i]->Update(dt);
		}
	}
}