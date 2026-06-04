#include "ImGuiHelpers.h"

#include "Core/Vector2D.h"
#include "imgui-master/imgui.h"

namespace ImGui
{
	void VectorText(const char* label, VectorF vector)
	{
		ImGui::Text("%s: %f, %f", label, vector.x, vector.y);
	}

	void VectorText(const char* label, VectorI vector)
	{
		ImGui::Text("%s: %d, %d", label, vector.x, vector.y);
	}

	void InputVectorF(const char* label, VectorF& vector)
	{
		ImGui::PushID(label);
		float vec[2] = { vector.x, vector.y };
		if (ImGui::InputFloat2(label, vec))
		{
			vector = VectorF(vec[0], vec[1]);
		}
		ImGui::PopID();
	}

	bool InputVectorI(const char* label, VectorI& vector)
	{
		bool did_change = false;

		ImGui::PushID(label);
		int vec[2] = { vector.x, vector.y };
		if (ImGui::InputInt2(label, vec))
		{
			vector = VectorI(vec[0], vec[1]);
			did_change = true;
		}
		ImGui::PopID();

		return did_change;
	}

	bool ActiveButton(const char* label, bool isActive)
	{
		if (!isActive)
		{
			ImGui::BeginDisabled();
		}

		bool did_press = ImGui::Button(label);

		if (!isActive)
		{
			ImGui::EndDisabled();
		}

		return did_press;
	}

	void DisplayRect(RectF rect)
	{
		ImGui::PushID((int)(rect.x1 + rect.x2 + rect.y1 + rect.y2));

		float rectxy1[2] = { rect.x1, rect.y1 };
		if (ImGui::InputFloat2("XY1", rectxy1))
		{
			rect.x1 = rectxy1[0];
			rect.y1 = rectxy1[1];
		}

		float rectxy2[2] = { rect.x2, rect.y2 };
		if (ImGui::InputFloat2("XY2", rectxy2))
		{
			rect.x2 = rectxy2[0];
			rect.y2 = rectxy2[1];
		}

		VectorF center = rect.Center();
		float width = rect.Width();
		float height = rect.Height();

		float rect_size[2] = { width, height };
		if (ImGui::InputFloat2("size", rect_size))
		{
			rect.SetSize(VectorF(rect_size[0], rect_size[1]));
			rect.SetCenter(center);
		}

		ImGui::PopID();
	}

	bool SpriteSheetCombo(StringBuffer64& selected)
	{
		bool did_select = false;

		// file select dropdown
		if (ImGui::BeginCombo("Texture", selected.c_str()))
		{
			FileManager* fm = FileManager::Get();
			std::vector<BasicString> file_names = fm->fileNamesInFolder(FileManager::Images);

			std::sort(file_names.begin(), file_names.end(), [](const BasicString& a, const BasicString& b) {
				
				StringBuffer64 str_a(a.c_str());
				str_a = str_a.to_lower();

				StringBuffer64 str_b(b.c_str());
				str_b = str_b.to_lower();

				int index = 0;
				while (str_a.length() > index && str_b.length() > index)
				{
					if (str_a.c_str()[index] < str_b.c_str()[index])
						return true;
					else if (str_a.c_str()[index] > str_b.c_str()[index])
						return false;

					index++;
				}
				return str_a.length() < str_b.length();
			});

			for (u32 i = 0; i < file_names.size(); i++)
			{
				const bool is_selected = StringCompare(selected.c_str(), file_names[i].c_str());

				if (ImGui::Selectable(file_names[i].c_str(), is_selected))
				{
					selected = file_names[i].c_str();
					did_select = true;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		if (ImGui::InputText("Custom Texture", selected.buffer(), selected.bufferLength()))
		{
			
		}

		return did_select;
	}
}