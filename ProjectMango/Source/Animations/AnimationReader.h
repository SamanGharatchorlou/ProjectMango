#pragma once

namespace ECS
{
	struct Animation;
}

namespace AnimationReader
{
	void BuildAnimatior(const char* file, std::vector<ECS::Animation>& animator);
};
