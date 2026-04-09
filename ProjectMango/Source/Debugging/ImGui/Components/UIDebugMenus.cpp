#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "ECS/Components/UIComponents.h"
#include "Input/InputManager.h"

ComponentID DebugMenu::DoUIButtonDebugMenu(ECS::Entity& entity)
{
	StringBuffer32 type_name = UIButton::TypeName();
	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		ECS::UIButton& ui_button = GetComponentRef(UIButton, entity);
		ImGui::PushID(entity + (int)UIButton::TypeId());

		ImGui::Text("Is Pressed %d", ui_button.IsPressed(c_inputBuffer));
		ImGui::Text("callback: %s", ui_button.callback.c_str());

		ImGui::PopID();
	}

	return UIButton::TypeId();
}

u32 DebugMenu::DoUITextDebugMenu(ECS::Entity& entity)
{
	StringBuffer32 type_name = UIText::TypeName();
	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		ECS::UIText& ui_text = GetComponentRef(UIText, entity);
		ImGui::PushID(entity + (int)UIText::TypeId());

		const char* text = ui_text.text.c_str() ? ui_text.text.c_str() : "no text";
		ImGui::Text("%s", ui_text.text.c_str() );

		ImGui::Text("Size: %d", ui_text.font.GetPtSize());
		ImGui::Text("callback: %s", ui_text.callback.c_str());

		ImGui::PopID();
	}

	return UIText::TypeId();
}
