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

				if(systems.entAndSystems[i]->paused)
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
								
				if(systems.entOrSystems[i]->paused)
					continue;

				systems.entOrSystems[i]->Update(dt);
			}
		}
	}

	void EntityCoordinator::SetSystemPaused(Archetype type, bool is_paused)
	{
		for( u32 i = 0; i < systems.entAndSystems.size(); i++ )
		{
			if(systems.entAndSystems[i]->signature == type)
			{
				systems.entAndSystems[i]->paused = is_paused;
				return;
			}
		}
		for( u32 i = 0; i < systems.entOrSystems.size(); i++ )
		{
			if(systems.entOrSystems[i]->signature == type)
			{
				systems.entOrSystems[i]->paused = is_paused;
				return;
			}
		}
	}

	
	void EntityCoordinator::ToggleSystemPaused(Archetype type)
	{
		for( u32 i = 0; i < systems.entAndSystems.size(); i++ )
		{
			if(systems.entAndSystems[i]->signature == type)
			{
				systems.entAndSystems[i]->paused = !systems.entAndSystems[i]->paused;
				return;
			}
		}
		for( u32 i = 0; i < systems.entOrSystems.size(); i++ )
		{
			if(systems.entOrSystems[i]->signature == type)
			{
				systems.entOrSystems[i]->paused = !systems.entOrSystems[i]->paused;
				return;
			}
		}
	}
}