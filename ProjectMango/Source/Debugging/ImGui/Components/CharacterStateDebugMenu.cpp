#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "ECS/Components/AIComponents.h"
#include "ECS/Components/Components.h"


u32 DebugMenu::DoEntityStateDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::EntityState;

	ImGui::PushID(entity + (int)type);
	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::EntityState& character_state = GetComponentRef(EntityState, entity);
		//ImGui::Text("Movement Input: %f, %f", character_state.movementInput.x, character_state.movementInput.y );

		if (ImGui::TreeNode("State Editor"))
		{
			if(ECS::AIController* aic = GetComponent(AIController, entity))
			{
				if (ImGui::BeginCombo("Enter Action", "", 0))
				{
					for( u32 i = 0; i < (u32)::Count; i++ )
					{
						const char* action_string = ActionToString((Action::Enum)i);
						if (ImGui::Selectable(action_string, false))
						{
							//aic->PushState((ActionState)i);
						}
					}
						
					ImGui::EndCombo();
				}
			}
			else if(ECS::PlayerController* pc = GetComponent(PlayerController, entity))
			{
				if (ImGui::BeginCombo("Enter Action", "", 0))
				{
					for( u32 i = 0; i < (u32)Action::Count; i++ )
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

	return (u32)type;
}