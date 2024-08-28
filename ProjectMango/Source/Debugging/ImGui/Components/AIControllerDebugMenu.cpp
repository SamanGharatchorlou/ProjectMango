#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/Components/AIController.h"
#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"

static bool s_enemyCanMove = true;
static bool s_showAttackColliders = true;

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
		if (ImGui::TreeNode("Displaay"))
		{
			ImGui::Checkbox("Allow Enemy Movement", &s_enemyCanMove);

			ImGui::Checkbox("Display Attack Colliders", &s_showAttackColliders);

			ImGui::TreePop();
		}
	}
		
	ImGui::PopID();

	return type;
}