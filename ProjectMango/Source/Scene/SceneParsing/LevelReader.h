#pragma once

namespace ECS
{
	struct Level;
}

namespace Level
{
	void BuildLevel(ECS::Level& level, u32 level_number);
}