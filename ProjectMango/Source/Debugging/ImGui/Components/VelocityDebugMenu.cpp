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
		if (ImGui::TreeNode("Component Data"))
		{
			ImGui::VectorText("Speed", physics.speed);
			ImGui::VectorText("Max Speed", physics.maxSpeed);
			ImGui::VectorText("Acceleration", physics.acceleration);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Display"))
		{
			if (!physics.speed.isZero())
			{
				ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
				ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);

				//VectorF A = transform.rect.Center();
				//VectorF B = A + physics.speed * state.movementDirection.toFloat() * 10.0f;

				//DebugDraw::Line(A, B, Colour::Green);
			}

			ImGui::TreePop();
		}
	}
	ImGui::PopID();

	return type;
}