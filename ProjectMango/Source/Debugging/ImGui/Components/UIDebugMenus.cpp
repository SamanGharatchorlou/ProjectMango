#include "pch.h"
#include "ComponentDebugMenu.h"

#include "imgui-master/imgui.h"
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
	if(ECS::UIText* ui_text = GetComponent(UIText, entity))
	{
		if (ImGui::CollapsingHeader(type_name.c_str()))
		{
			ImGui::PushID(entity + (int)UIText::TypeId());

			//const char* text = ui_text->text.c_str() ? ui_text->text.c_str() : "no text";
			//ImGui::Text("%s", ui_text->text.c_str());

			StringBuffer64 text_input = ui_text->text.c_str();
			if (ImGui::InputText("", text_input.buffer(), text_input.bufferLength()))
			{
				ui_text->SetText(text_input.c_str());
			}

			ImGui::Text("Size: %d", ui_text->font.GetPtSize());

			//char buffer[32];
			//snprintf("Pt size: %")
			int pt_size = ui_text->font.GetPtSize();
			if (ImGui::SliderInt("Pt size", &pt_size, 0, 50))
			{
				ui_text->font.SetSize(ui_text->text.c_str(), pt_size);
			}

			ImGui::Text("callback: %s", ui_text->callback.c_str());

			ImGui::PopID();
		}
	}
	else
	{
		if (ImGui::Button("Add UIText component"))
			AddComponent(UIText, entity);
	}

	return UIText::TypeId();
}
