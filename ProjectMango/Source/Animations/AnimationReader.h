#pragma once

namespace ECS
{
	struct Animation;
}

namespace AnimationReader
{
	void BuildAnimatior(ECS::Entity entity, const char* file);
};
