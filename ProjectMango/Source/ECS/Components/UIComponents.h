#pragma once

#include "UI/Text/Font.h"

class Cursor;

namespace ECS
{
	struct UICursor
	{
		COMPONENT_TYPE(UICursor)

		Cursor* cursor;
		
		UICursor();

		static UICursor* Get();

		VectorF Position() const;
	};

	struct UIButton
	{
		COMPONENT_TYPE(UIButton);
		
		BasicString callback;

		int lastPressedFrameCount;

		bool toggle;
		
		UIButton();
		bool IsPressed(int frame_buffer = 0) const;
	};

	struct UIText
	{
		COMPONENT_TYPE(UIText);

		BasicString callback;

		BasicString text;
		Font font;

		// useful when part of another thing that has a sprite and we want to draw center to it
		VectorF renderOffset;
		bool center;
		
		UIText();

		void SetText(const char* text);
		void SetColour(SColour scolour);

		void SetSize(int ptsize);
		void FitToSize(VectorF size);

		void SetRenderOffsetToCenter();

		void Render();
	};
}