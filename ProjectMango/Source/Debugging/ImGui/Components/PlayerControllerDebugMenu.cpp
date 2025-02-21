#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"

ECS::Component::Type DebugMenu::DoPlayerControllerDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::PlayerController;

	//if (HasComponent(entity, type))
	{
		if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
		{
			ImGui::PushID(entity + (int)type);
			if (ImGui::TreeNode("Component Data"))
			{
				//ECS::PlayerController& pc = GetComponentRef(PlayerController, entity);
				ECS::CharacterState& state = GetComponentRef(CharacterState, entity);

				//ImGui::VectorText("Movement Direction", state.movementDirection);
				//ImGui::VectorText("Facing Direction", state.facingDirection);

				//ImGui::Text("State count: %d", pc.actions.stack.size());
				//ImGui::Text("Current State: %s", actionToString(pc.actions.Top().action).c_str());


				ImGui::TreePop();
			}
			
			ImGui::PopID();
		}
	}

	return type;
}