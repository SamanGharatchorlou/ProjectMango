#pragma once

namespace ECS
{
	struct Animator;
}

namespace AnimationReader
{
	//void buildAnimator(const XMLNode animationNode, Animator& out_animator);
	//void Parse(const char* animation, Animator& animator);

	void BuildAnimatior(const char* file, ECS::Animator& animator);
};
