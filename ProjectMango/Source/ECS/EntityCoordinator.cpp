#include "pch.h"
#include "EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"

namespace ECS
{
	void EntityCoordinator::UpdateSystems(float dt)
	{
		bool game_state_active = false;
		if(DebugMenu::GetGamePlayerState().isActive)
		{
			game_state_active = !DebugMenu::GetGamePlayerState().nextFrame;
			DebugMenu::GetGamePlayerState().nextFrame = false;
			dt = 1.0f / 60.0f;
		}

		for (size_t i = 0; i < systems.entAndSystems.size(); i++)
		{
			if(game_state_active)
			{
				if(i != 1 && i != 4 && i != 5)
					continue;
			}

			systems.entAndSystems[i]->Update(dt);
		}

		for (size_t i = 0; i < systems.entOrSystems.size(); i++)
		{
			systems.entOrSystems[i]->Update(dt);
		}
	}
}