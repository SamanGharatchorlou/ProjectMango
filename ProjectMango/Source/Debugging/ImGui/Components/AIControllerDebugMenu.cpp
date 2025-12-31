#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/Components/AIComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "ECS/Components/Components.h"

using namespace ECS;

u32 DebugMenu::DoAIControllerDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::AIController;

	ImGui::PushID(entity + (int)type);
	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
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

	return (u32)type;
}

u32 DebugMenu::DoPathingDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::Pathing;

	ImGui::PushID(entity + (int)type);
	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::Pathing& pathing = GetComponentRef(Pathing, entity);

		ImGui::Text("Has valid path: ", GetBoolString(pathing.hasValidPath));
	}
	ImGui::PopID();

	return (u32)type;
}

u32 DebugMenu::DoBehaviourStateDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::BehaviourState;

	ImGui::PushID(entity + (int)type);
	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		//ECS::BehaviourState& behav = GetComponentRef(BehaviourState, entity);


		//ImGui::Text("Current: ", ActionToString(behav. );
	}
	ImGui::PopID();

	return (u32)type;
}


u32 DebugMenu::DoEntityStateDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::EntityState;

	ImGui::PushID(entity + (int)type);
	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
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

	return (u32)type;
}