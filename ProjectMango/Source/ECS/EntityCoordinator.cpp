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

		for( u32 order = 0; order < systems.entAndSystems.size() + systems.entOrSystems.size(); order++ )
		{
			for (size_t i = 0; i < systems.entAndSystems.size(); i++)
			{
				if(systems.entAndSystems[i]->orderIndex != order)
					continue;

				if(game_state_active)
				{
					// ignore all systems except rendering
					if( i < systems.entAndSystems.size() - 4 )
						continue;
				}

				systems.entAndSystems[i]->Update(dt);
			}

			for (size_t i = 0; i < systems.entOrSystems.size(); i++)
			{
				if(systems.entOrSystems[i]->orderIndex != order)
					continue;

				systems.entOrSystems[i]->Update(dt);
			}
		}
	}
}