#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "ECS/Components/AIController.h"
#include "ECS/Components/PlayerController.h"
#include "Animations/CharacterStates.h"


ECS::Component::Type DebugMenu::DoCharacterStateDebugMenu(ECS::Entity& entity)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Component::Type type = ECS::Component::CharacterState;

	ImGui::PushID(entity + (int)type);
	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::CharacterState& character_state = ecs->GetComponentRef(CharacterState, entity);
		ImGui::Text("Movement Input: %f, %f", character_state.movementInput.x, character_state.movementInput.y );

		if (ImGui::TreeNode("State Editor"))
		{
			if(ECS::AIController* aic = ecs->GetComponent(AIController, entity))
			{
				if (ImGui::BeginCombo("Enter Action", "", 0))
				{
					for( u32 i = 0; i < (u32)ActionState::Count; i++ )
					{
						const char* action_string = ActionToString((ActionState)i);
						if (ImGui::Selectable(action_string, false))
						{
							//aic->PushState((ActionState)i);
						}
					}
						
					ImGui::EndCombo();
				}
			}
			else if(ECS::PlayerController* pc = ecs->GetComponent(PlayerController, entity))
			{
				if (ImGui::BeginCombo("Enter Action", "", 0))
				{
					for( u32 i = 0; i < (u32)ActionState::Count; i++ )
					{
						//const char* action_string = actionToString((ActionState)i).c_str();
						//if (ImGui::Selectable(action_string, false))
						//{
						//	pc->PushState((ActionState)i);
						//}
					}
						
					ImGui::EndCombo();
				}
			}

			ImGui::TreePop();
		}
	}
	ImGui::PopID();

	return type;
}