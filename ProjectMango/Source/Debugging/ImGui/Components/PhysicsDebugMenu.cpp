#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/PlayerController.h"

ECS::Component::Type DebugMenu::DoPhysicsDebugMenu(ECS::Entity& entity)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Component::Type type = ECS::Component::Physics;

	ImGui::PushID(entity + (int)type);
	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::Physics& physics = ecs->GetComponentRef(Physics, entity);

		ImGui::VectorText("Speed", physics.speed);
		ImGui::VectorText("Max Speed", physics.maxSpeed);
		ImGui::VectorText("Acceleration", physics.acceleration);
		ImGui::Text("Is On Floor: %d", physics.onFloor);
	}
	ImGui::PopID();

	return type;
}