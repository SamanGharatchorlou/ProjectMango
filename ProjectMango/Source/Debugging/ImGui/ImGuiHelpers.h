#pragma once

namespace ImGui
{
	void VectorText(const char* label, VectorF vector);
	void VectorText(const char* label, VectorI vector);
	void InputVectorF(const char* label, VectorF& vector);
	void InputVectorI(const char* label, VectorI& vector);
	bool ActiveButton(const char* label, bool isActive);
	void DisplayRect(RectF rect);

	bool SpriteSheetCombo(StringBuffer64& selected);
}