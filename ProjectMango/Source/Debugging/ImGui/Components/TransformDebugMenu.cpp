#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "imgui-master/imgui.h"
#include "Debugging/ImGui/ImGuiHelpers.h"

bool s_displayRect = false;
bool s_displayPosition = false;

ECS::Component::Type DebugMenu::DoTransformDebugMenu(ECS::Entity& entity)
{
    ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Component::Type type = ECS::Component::Transform;

	ImGui::PushID(entity + (int)type);
	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);

		//if (ImGui::TreeNode("Component Data"))
		//{
		//	ImGui::DisplayRect(transform.rect);

		//	if (ImGui::Button("Vertical Flip"))
		//	{
		//		transform.flip = transform.flip == SDL_FLIP_HORIZONTAL ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
		//	}

		//	ImGui::TreePop();
		//}

		if (ImGui::TreeNode("Display"))
		{
			ImGui::Checkbox("Display Object Rect", &s_displayRect);
			if (s_displayRect)
			{
				DebugDraw::RectOutline(transform.rect, Colour::Green);
			}

			ImGui::Checkbox("Display Position", &s_displayPosition);
			if (s_displayPosition)
			{
				DebugDraw::Point(transform.GetPos(), 4.0f, Colour::Green);
			}

			ImGui::TreePop();
		}
	}
	ImGui::PopID();

	return type;
}