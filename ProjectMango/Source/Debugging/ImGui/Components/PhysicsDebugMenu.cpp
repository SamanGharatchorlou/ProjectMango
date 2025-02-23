#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "ECS/Components/Physics.h"

u32 DebugMenu::DoPhysicsDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::Physics;

	ImGui::PushID(entity + (int)type);
	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::Physics& physics = GetComponentRef(Physics, entity);

		ImGui::VectorText("Speed", physics.speed);
		ImGui::VectorText("Max Speed", physics.maxSpeed);
		ImGui::VectorText("Acceleration", physics.acceleration);
		ImGui::Text("Is On Floor: %d", physics.onFloor);
	}
	ImGui::PopID();

	return (u32)type;
}