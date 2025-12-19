#include "pch.h"
#include "PlayerControllerSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "Input/InputManager.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Biome.h"
#include "Game/Camera/Camera.h"


namespace ECS
{
	void PlayerControllerSystem::Update(float dt)
	{
		
		InputManager* input = InputManager::Get();

		for (Entity entity : entities)
		{
			PlayerController& pc = GetComponentRef(PlayerController, entity);
		}
	} 
}

