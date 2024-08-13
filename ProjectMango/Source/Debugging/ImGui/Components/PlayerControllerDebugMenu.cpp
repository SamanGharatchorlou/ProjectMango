#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/Components/PlayerController.h"
#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"

ECS::Component::Type DebugMenu::DoPlayerControllerDebugMenu(ECS::Entity& entity)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Component::Type type = ECS::Component::PlayerController;

	//if (ecs->HasComponent(entity, type))
	{
		if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
		{
			ImGui::PushID(entity + (int)type);
			if (ImGui::TreeNode("Component Data"))
			{
				ECS::PlayerController& pc = ecs->GetComponentRef(PlayerController, entity);
				ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

				ImGui::VectorText("Movement Direction", state.movementDirection);
				ImGui::VectorText("Facing Direction", state.facingDirection);

				ImGui::Text("State count: %d", pc.actions.stack.size());
				ImGui::Text("Current State: %s", actionToString(pc.actions.Top().action).c_str());


				ImGui::TreePop();
			}
			
			ImGui::PopID();
		}
	}

	return type;
}