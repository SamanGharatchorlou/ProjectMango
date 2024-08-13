#pragma once

#include "Core/Vector2D.h"
#include "imgui-master/imgui.h"

namespace ImGui
{
	static void VectorText(const char* label, VectorF vector)
	{
		ImGui::Text("%s: %f, %f", label, vector.x, vector.y);
	}

	static void VectorText(const char* label, Vector2D<int> vector)
	{
		ImGui::Text("%s: %d, %d", label, vector.x, vector.y);
	}

	static void InputVectorF(const char* label, VectorF& vector)
	{
		ImGui::PushID(label);
		float vec[2] = { vector.x, vector.y };
		if (ImGui::InputFloat2(label, vec))
		{
			vector = VectorF(vec[0], vec[1]);
		}
		ImGui::PopID();
	}

	static void InputVectorI(const char* label, VectorI& vector)
	{
		ImGui::PushID(label);
		int vec[2] = { vector.x, vector.y };
		if (ImGui::InputInt2(label, vec))
		{
			vector = VectorI(vec[0], vec[1]);
		}
		ImGui::PopID();
	}

	static bool ActiveButton(const char* label, bool isActive)
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

	static void DisplayRect(RectF rect)
	{
		ImGui::PushID((int)(rect.x1 + rect.x2 + rect.y1 + rect.y2));

		static float rectxy1[2] = { rect.x1, rect.y1 };
		if (ImGui::InputFloat2("XY1", rectxy1))
		{
			rect.x1 = rectxy1[0];
			rect.y1 = rectxy1[1];
		}

		static float rectxy2[2] = { rect.x2, rect.y2 };
		if (ImGui::InputFloat2("XY2", rectxy2))
		{
			rect.x2 = rectxy2[0];
			rect.y2 = rectxy2[1];
		}

		VectorF center = rect.Center();
		float width = rect.Width();
		float height = rect.Height();

		static float rect_size[2] = { width, height };
		if (ImGui::InputFloat2("size", rect_size))
		{
			rect.SetSize(VectorF(rect_size[0], rect_size[1]));
			rect.SetCenter(center);
		}

		ImGui::PopID();
	}

	static void DoDebugRenderTypeDropDown(DebugDrawType& type) 
	{
        const char* arrayDebugDrawType[4] = { "Point", "Line", "RectOutline", "RectFill"};
		ImGui::Combo("Draw Types", (int*)&type, arrayDebugDrawType, 4);
	}
}