#pragma once

namespace ECS { struct Collider; }

namespace DebugMenu 
{
	u32 DoEntityDataDebugMenu(ECS::Entity& entity);
	u32 DoTransformDebugMenu(ECS::Entity& entity);
	u32 DoSpriteDebugMenu(ECS::Entity& entity);
	u32 DoCharacterStateDebugMenu(ECS::Entity& entity);
	u32 DoPhysicsDebugMenu(ECS::Entity& entity);
	u32 DoAnimatorDebugMenu(ECS::Entity& entity);
	u32 DoColliderDebugMenu(ECS::Entity& entity);
	u32 DoPlayerControllerDebugMenu(ECS::Entity& entity);
	u32 DoPathingDebugMenu(ECS::Entity& entity);
	u32 DoAIControllerDebugMenu(ECS::Entity& entity);
	u32 DoHealthDebugMenu(ECS::Entity& entity);
	u32 DoBiomeDebugMenu(ECS::Entity& entity);
	u32 DoArmDebugMenu(ECS::Entity& entity);
	u32 DoUIButtonDebugMenu(ECS::Entity& entity);
	u32 DoUITextDebugMenu(ECS::Entity& entity);
	u32 DoCoinStackDebugMenu(ECS::Entity& entity);
	u32 DoInventoryDebugMenu(ECS::Entity& entity);
	u32 DoCardDebugMenu(ECS::Entity& entity);
	u32 DoColourDebugMenu(ECS::Entity& entity);

	void DrawCollider(const ECS::Collider& collider);

	static inline const char* GetBoolString(bool value)
	{
		return value ? "true" : "false";
	}
}