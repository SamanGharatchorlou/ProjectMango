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

ECS::Component::Type DebugMenu::DoTransformDebugMenu(ECS::Entity& entity)
{
    ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Component::Type type = ECS::Component::Transform;

	ImGui::PushID(entity + (int)type);
	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);

		ImGui::Checkbox("Display Object Rect", &s_displayRect);
		if (s_displayRect)
		{
			RectF rect(transform.worldPosition, transform.size);
			DebugDraw::RectOutline(rect, Colour::Blue);
		}

		ImGui::Checkbox("Display Character Position", &s_displayCharacterPosition);
		if (s_displayCharacterPosition)
		{
			VectorF position = transform.GetCharacterCenter();
			DebugDraw::Point(position, Colour::Green);
		}

		ImGui::Checkbox("Display Flip Point", &s_displayFlipPoint);
		if (s_displayFlipPoint)
		{
			VectorF flip_point = transform.worldPosition + transform.size / 2.0f;
			if(ECS::Sprite* sprite = ecs->GetComponent(Sprite, entity))
				flip_point = (sprite->flipPoint * transform.size) + transform.worldPosition;

			DebugDraw::Point(flip_point, Colour::Red);
		}

		ImGui::Checkbox("Output Position", &s_outputPosition);
		if(s_outputPosition)
		{
			DebugPrint(PriorityLevel::Log, "Position: %f, %f", transform.worldPosition.x, transform.worldPosition.y);
		}
	}
	ImGui::PopID();

	return type;
}
