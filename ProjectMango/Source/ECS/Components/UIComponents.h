#pragma once

//#include "Input/Button.h"

class Cursor;

namespace ECS
{
	struct UICursor
	{
		COMPONENT_TYPE(UICursor)

		~UICursor();

		Cursor* cursor;

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
		COMPONENT_TYPE(UIText);

	};
}