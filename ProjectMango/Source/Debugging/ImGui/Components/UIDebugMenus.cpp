#include "pch.h"
#include "ComponentDebugMenu.h"

#include "imgui-master/imgui.h"
#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "ECS/Components/UIComponents.h"
#include "Input/InputManager.h"

ComponentID DebugMenu::DoUIButtonDebugMenu(ECS::Entity& entity)
{
	COMPONENT_PREAMBLE(UIButton);

	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		ECS::UIButton& ui_button = GetComponentRef(UIButton, entity);
		ImGui::PushID(entity + type_id);

		ImGui::Text("Is Pressed %d", ui_button.IsPressed(c_inputBuffer));

		StringBuffer64 callback_input = ui_button.callback.c_str();
		if (ImGui::InputText("Callback", callback_input.buffer(), callback_input.bufferLength()))
		{
			ui_button.callback = callback_input.c_str();
		}

		ImGui::PopID();
	}

	return type_id;
}

u32 DebugMenu::DoUITextDebugMenu(ECS::Entity& entity)
{
	COMPONENT_PREAMBLE(UIText);

	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		UIText& ui_text = GetComponentRef(UIText, entity);

		ImGui::PushID(entity + (int)type_id);

		StringBuffer64 text_input = ui_text.text.c_str();
		if (ImGui::InputText("Text", text_input.buffer(), text_input.bufferLength()))
		{
			ui_text.SetText(text_input.c_str());
		}

		StringBuffer64 callback_input = ui_text.callback.c_str();
		if (ImGui::InputText("Callback", callback_input.buffer(), callback_input.bufferLength()))
		{
			ui_text.callback = callback_input.c_str();
		}

		if (ImGui::Checkbox("Center", &ui_text.center))
		{
			ui_text.UpdateRenderOffset();
		}

		if (ImGui::Checkbox("Wrapped", &ui_text.font.wrapped))
		{
			ui_text.UpdateRenderOffset();
		}

		int pt_size = ui_text.font.GetPtSize();
		if (ImGui::SliderInt("Pt size", &pt_size, 0, 50))
		{
			ui_text.font.SetSize(ui_text.text.c_str(), pt_size);
		}

		ImGui::PopID();
	}

	return type_id;
}
