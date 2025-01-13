#pragma once

namespace ECS
{
	struct Cursor
	{
		COMPONENT_TYPE(Cursor)

		Entity target = EntityInvalid;

	};
}