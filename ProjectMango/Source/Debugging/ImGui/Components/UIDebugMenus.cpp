#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "ECS/Components/UIComponents.h"
#include "Input/InputManager.h"

u32 DebugMenu::DoUIButtonDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::UIButton;

	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::UIButton& ui_button = GetComponentRef(UIButton, entity);
		ImGui::PushID(entity + (int)type);

		ImGui::Text("Is Pressed %d", ui_button.IsPressed(c_inputBuffer));

		ImGui::PopID();
	}

	return (u32)type;
}
