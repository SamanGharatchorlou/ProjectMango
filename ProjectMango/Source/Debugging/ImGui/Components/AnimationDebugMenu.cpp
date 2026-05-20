#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/Components/GraphicComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Game/FrameRateController.h"
#include "imgui-master/imgui.h"

static Action::Enum s_activeAction = Action::None;

std::vector<BasicString> s_animationLog;

u32 DebugMenu::DoAnimatorDebugMenu(ECS::Entity& entity)
{
	StringBuffer32 type_name = Animator::TypeName();
	ComponentID type_id = Animator::TypeId();

	if (!HasComponent(Transform, entity))
		return type_id;

	ImGui::PushID(entity + type_id);
	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		const ECS::Animator& animator = GetComponentRef(Animator, entity);

		Action::Enum active_animation = animator.GetActiveAnimation()->action;
		if(active_animation != s_activeAction)
		{
			s_activeAction = active_animation;

			const FrameRateController& frc = FrameRateController::Get();
			char new_string_log[128];

			const char* action = ActionToString(active_animation);
			float start_time = frc.GameSeconds();

			snprintf(new_string_log, 128, "Action: %s | Time: %f", action, start_time);
			s_animationLog.push_back(new_string_log);
		}

		for( u32 i = 0; i < s_animationLog.size(); i++ )
		{
			ImGui::Text(s_animationLog[i].c_str());
		}

		ImGui::Text("Loops: %d", animator.loopCount);
	}

	ImGui::PopID();

	return type_id;
}