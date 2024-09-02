#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/Components/AIController.h"
#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"

static bool s_enemyCanMove = true;

bool EnemyCanMove() 
{ 
	return s_enemyCanMove; 
}

ECS::Component::Type DebugMenu::DoAIControllerDebugMenu(ECS::Entity& entity)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Component::Type type = ECS::Component::AIController;

	ImGui::PushID(entity + (int)type);
	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::AIController& aic = ecs->GetComponentRef(AIController, entity);

		ImGui::Checkbox("Allow Enemy Movement", &s_enemyCanMove);

		ImGui::Text("Has target: %s", ecs->GetEntityName(aic.target));

		ImGui::Text("Distance to target %f", aic.DistanceToTarget());
	}
		
	ImGui::PopID();

	return type;
}