#pragma once

namespace ECS { struct AIIntent; }

namespace AIStrategies
{
	void SimpleAttacker(ECS::Entity entity, ECS::AIIntent& intent);

	void ShockSweeper(ECS::Entity entity, ECS::AIIntent& intent);
}