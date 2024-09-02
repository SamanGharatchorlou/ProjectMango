#pragma once

namespace ECS
{

	struct AIController
	{
		COMPONENT_TYPE(AIController)

		Entity target = EntityInvalid;

		bool moveToTarget = true;
	};
}