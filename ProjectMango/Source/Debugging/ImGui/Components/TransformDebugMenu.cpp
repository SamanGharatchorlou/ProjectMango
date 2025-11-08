#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "imgui-master/imgui.h"
#include "Debugging/ImGui/ImGuiHelpers.h"

bool s_displayRect = false;
bool s_displayCharacterPosition = false;
bool s_displayFlipPoint = false;
bool s_outputPosition = false;

u32 DebugMenu::DoTransformDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::Transform;

	ImGui::PushID(entity + (int)type);
	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::Transform& transform = GetComponentRef(Transform, entity);
		ImGui::Text("World Position: %f, %f", transform.worldPosition.x, transform.worldPosition.y);

		ImGui::Checkbox("Display Object Rect", &s_displayRect);
		if (s_displayRect)
		{
			RectF rect(transform.worldPosition, transform.size);
			DebugDraw::RectOutline(rect, SColour::Blue);
		}

		ImGui::Checkbox("Display Character Position", &s_displayCharacterPosition);
		if (s_displayCharacterPosition)
		{
			VectorF position = transform.GetObjectCenter();
			DebugDraw::Point(position, SColour::Green);
		}

		ImGui::Checkbox("Display Flip Point", &s_displayFlipPoint);
		if (s_displayFlipPoint)
		{
			VectorF flip_point = transform.worldPosition + transform.size / 2.0f;
			if(ECS::Sprite* sprite = GetComponent(Sprite, entity))
				flip_point = (sprite->flipPoint * transform.size) + transform.worldPosition;

			DebugDraw::Point(flip_point, SColour::Red);
		}

		ImGui::Checkbox("Output Position", &s_outputPosition);
		if(s_outputPosition)
		{
			DebugPrint(PriorityLevel::Log, "Position: %f, %f", transform.worldPosition.x, transform.worldPosition.y);
		}
	}
	ImGui::PopID();

	return (u32)type;
}

u32 DebugMenu::DoArmDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::Arm;

	if (!HasComponent(Arm, entity))
		return (u32)type;

	ImGui::PushID(entity + (int)type);
	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		const ECS::Arm& arm = GetComponentRef(Arm, entity);
			
		if (ImGui::TreeNode("Component Data"))
		{
			ImGui::Text("Flip Point: %f, %f", arm.anchorPoint.x, arm.anchorPoint.y);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Display"))
		{
			ImGui::Checkbox("Display Flip Point", &s_displayFlipPoint);
			if (s_displayFlipPoint)
			{
				ECS::Transform& transform = GetComponentRef(Transform, entity);
				VectorF anchor_point = arm.anchorPoint;

				if(ECS::Sprite* sprite = GetComponent(Sprite, entity))
				{
					if(sprite->IsFlipped())
						anchor_point = VectorF(1.0f,1.0f) - anchor_point;
				}
				
				VectorF flip_point = transform.GetRelativePosition(anchor_point); // * transform.size) + transform.worldPosition;
				DebugDraw::Point(flip_point, SColour::Red, 2.0);
			}

			ImGui::TreePop();
		}
	}

	ImGui::PopID();

	return (u32)type;
}