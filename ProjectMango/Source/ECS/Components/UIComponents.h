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
	};

	struct UIButton
	{
		COMPONENT_TYPE(UIButton);


	};
}