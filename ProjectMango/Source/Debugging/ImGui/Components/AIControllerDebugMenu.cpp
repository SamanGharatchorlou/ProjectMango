#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/Components/AIComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "ECS/Components/Components.h"

using namespace ECS;

u32 DebugMenu::DoAIControllerDebugMenu(ECS::Entity& entity)
{
	StringBuffer32 type_name = AIController::TypeName();
	ComponentID type_id = AIController::TypeId();

	ImGui::PushID(entity + (int)type_id);
	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		ECS::AIController& aic = GetComponentRef(AIController, entity);
		
		Entity target = Faction::GetTarget(entity);

		const VectorF distance = GetPosition(entity) - GetPosition(target);
		const float target_distance = distance.length();

		ImGui::Text("Has target: %s", ECS::GetName(target));
		ImGui::Text("Distance to target %f", target_distance);

		//ImGui::Text("Can move to target: %s", aic.canMoveToTarget ? "true" : "false");
	}
		
	ImGui::PopID();

	return type_id;
}

u32 DebugMenu::DoPathingDebugMenu(ECS::Entity& entity)
{
	StringBuffer32 type_name = Pathing::TypeName();
	ComponentID type_id = Pathing::TypeId();

	ImGui::PushID(entity + (int)type_id);
	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		ECS::Pathing& pathing = GetComponentRef(Pathing, entity);

		ImGui::Text("Has valid path: ", GetBoolString(pathing.hasValidPath));
	}
	ImGui::PopID();

	return type_id;
}

u32 DebugMenu::DoBehaviourStateDebugMenu(ECS::Entity& entity)
{
	//StringBuffer32 type_name = AIController::TypeName();
	ComponentID type_id = BehaviourState::TypeId();

	//ImGui::PushID(entity + (int)type);
	//if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	//{
	//	//ECS::BehaviourState& behav = GetComponentRef(BehaviourState, entity);


	//	//ImGui::Text("Current: ", ActionToString(behav. );
	//}
	//ImGui::PopID();

	return type_id;
}


u32 DebugMenu::DoEntityStateDebugMenu(ECS::Entity& entity)
{
	StringBuffer32 type_name = EntityState::TypeName();
	ComponentID type_id = EntityState::TypeId();

	ImGui::PushID(entity + (int)type_id);
	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		ECS::EntityState& state = GetComponentRef(EntityState, entity);

		ImGui::Text("Current: %s", ActionToString( state.current ));
		ImGui::Text("Next: %s", ActionToString( state.next ));
		
		ImGui::Text("Backlog");
		for( u32 i = 0; i < state.backlog.size(); i++ )
		{
			ImGui::Text("%d: %s", i, ActionToString( state.backlog[i] ));

		}
	}
	ImGui::PopID();

	return type_id;
}