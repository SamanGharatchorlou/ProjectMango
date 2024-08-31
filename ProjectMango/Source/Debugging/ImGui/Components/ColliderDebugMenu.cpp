#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"

ECS::Component::Type DebugMenu::DoColliderDebugMenu(ECS::Entity& entity)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Component::Type type = ECS::Component::Collider;

	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::Collider& collider = ecs->GetComponentRef(Collider, entity);
		ImGui::PushID(entity + (int)type);

		DebugDraw::RectOutline(collider.rect, Colour::Purple);
			
		ImGui::PopID();
	}

	return type;
}