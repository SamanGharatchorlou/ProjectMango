#pragma once

namespace ECS
{
	struct Animator;
}

namespace AnimationReader
{
	void BuildAnimatior(ECS::Animator& animator, const char* file);
};
