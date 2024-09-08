#pragma once

namespace ECS
{
	struct PlayerController
	{
		COMPONENT_TYPE(PlayerController)

		bool canEnterHover = true;
	};
}