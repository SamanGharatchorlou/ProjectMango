#pragma once

#include "UI/Text/Font.h"

class Cursor;

namespace ECS
{
	struct UICursor
	{
		COMPONENT_TYPE(UICursor)

		Cursor* cursor;
		
		~UICursor();

		static UICursor* Get();

		VectorF Position() const;
	};

	struct UIButton
	{
		COMPONENT_TYPE(UIButton);

		int lastPressedFrameCount;

		bool IsPressed(int frame_buffer = 0) const;
	};

	struct UIText
	{
		static constexpr int c_defaultFontSize = 10;
		static constexpr const char* c_defaultFont = "default";

		COMPONENT_TYPE(UIText);

		BasicString UID;
		//BasicString text;
		Font font;
		
		void SetText(const char* text);
		void SetColour(SColour::Enum colour);
		void Render();
	};
}