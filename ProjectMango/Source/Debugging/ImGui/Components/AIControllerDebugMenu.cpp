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
		if (ImGui::TreeNode("Component Data"))
		{
			ECS::AIController& pc = ecs->GetComponentRef(AIController, entity);
			ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

			ImGui::VectorText("Movement Direction", state.movementDirection);
			ImGui::VectorText("Facing Direction", state.facingDirection);

			ImGui::Text("State count: %d", pc.actions.stack.size());
			ImGui::Text("Current State: %s", actionToString(pc.actions.Top().action).c_str());

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Displaay"))
		{
			ImGui::Checkbox("Allow Enemy Movement", &s_enemyCanMove);

			ImGui::Checkbox("Display Attack Colliders", &s_showAttackColliders);
			if(s_showAttackColliders)
			{
				for( u32 i = 0; i < ECS::Direction::Count; i++ )
				{
					VectorI direction = ECS::s_directions[i];
					RectF rect = Enemy::GetAttackRect(entity, direction);
					DebugDraw::RectOutline(rect, Colour::Blue);
				}
			}

			ImGui::TreePop();
		}


	}
		
	ImGui::PopID();

	return type;
}