#pragma once

#include "ECS/Components/Components.h"
#include "ECS/Components/Collider.h"

namespace DebugMenu 
{
	ECS::Component::Type DoEntityDataDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoTransformDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoSpriteDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoCharacterStateDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoPhysicsDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoAnimatorDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoColliderDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoPlayerControllerDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoPathingDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoAIControllerDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoHealthDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoBiomeDebugMenu(ECS::Entity& entity);
}