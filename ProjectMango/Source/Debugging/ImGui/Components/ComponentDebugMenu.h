#pragma once

using namespace ECS;

namespace ECS { struct Collider; }

namespace DebugMenu 
{
#define COMPONENT_PREAMBLE(component)							\
	StringBuffer32 type_name = component::TypeName();			\
	ComponentID type_id = component::TypeId();					\
	ImGui::PushID(type_name.c_str());							\
	if(!HasComponent(component, entity)) {						\
		char buf[64]{ 0 }; \
		sprintf(buf, "Add %s component", component::TypeName().c_str()); \
		if (ImGui::Button(buf)) { AddComponent(component, entity); } \
		ImGui::PopID();			\
		return EntityInvalid; } \
	if (ImGui::Button("-")) { RemoveComponent(component, entity); ImGui::PopID(); return EntityInvalid; } \
	ImGui::PopID(); ImGui::SameLine();											\


	u32 DoEntityDataDebugMenu(ECS::Entity& entity);
	u32 DoTransformDebugMenu(ECS::Entity& entity);
	u32 DoSpriteDebugMenu(ECS::Entity& entity);
	u32 DoPhysicsDebugMenu(ECS::Entity& entity);
	u32 DoAnimatorDebugMenu(ECS::Entity& entity);
	u32 DoColliderDebugMenu(ECS::Entity& entity);
	u32 DoPlayerControllerDebugMenu(ECS::Entity& entity);
	u32 DoPathingDebugMenu(ECS::Entity& entity);
	u32 DoAIControllerDebugMenu(ECS::Entity& entity);
	u32 DoHealthDebugMenu(ECS::Entity& entity);
	u32 DoBiomeDebugMenu(ECS::Entity& entity);
	u32 DoUIButtonDebugMenu(ECS::Entity& entity);
	u32 DoUITextDebugMenu(ECS::Entity& entity);
	u32 DoCoinStackDebugMenu(ECS::Entity& entity);
	u32 DoInventoryDebugMenu(ECS::Entity& entity);
	u32 DoCardDebugMenu(ECS::Entity& entity);
	u32 DoBehaviourStateDebugMenu(ECS::Entity& entity);
	u32 DoEntityStateDebugMenu(ECS::Entity& entity);
	u32 DoFactionDebugMenu(ECS::Entity& entity);
	u32 DoAIIntentDebugMenu(ECS::Entity& entity);
	u32 DoCallbackDebugMenu(ECS::Entity& entity);

	inline void DoUIEditorMenus(ECS::Entity& entity)
	{
		DoEntityDataDebugMenu(entity);
		DoTransformDebugMenu(entity);
		DoSpriteDebugMenu(entity);
		DoUITextDebugMenu(entity);
		DoUIButtonDebugMenu(entity);
		DoCallbackDebugMenu(entity);
	}

	void DrawCollider(const ECS::Collider& collider);

	static inline const char* GetBoolString(bool value)
	{
		return value ? "true" : "false";
	}



}