#pragma once

//#include "Input/Button.h"

class Cursor;

namespace ECS
{
	struct UICursor
	{
		COMPONENT_TYPE(UICursor)

		Cursor* cursor;
	};
}