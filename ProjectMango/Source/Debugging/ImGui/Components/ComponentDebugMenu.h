#pragma once

#include "ECS/Components/Components.h"
#include "ECS/Components/Collider.h"

namespace DebugMenu 
{
	bool UsingPlaylist(ECS::Entity& entity);
	bool DisplayRenderRect(ECS::Entity& entity);
	bool SpriteFlipOverride(ECS::Entity& entity, SDL_RendererFlip& sprite_flip);
	bool RemovePath(ECS::Entity& entity);

	void SetPathAttackPoint(VectorF point, Colour colour, ECS::Direction direction);

	ECS::Component::Type DoTransformDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoSpriteDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoCharacterStateDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoPhysicsDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoAnimationDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoTileMapDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoColliderDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoPlayerControllerDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoPathingDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoAIControllerDebugMenu(ECS::Entity& entity);
	ECS::Component::Type DoHealthDebugMenu(ECS::Entity& entity);
}