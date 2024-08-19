#pragma once

namespace ECS
{
	struct Animation;
}

namespace AnimationReader
{
	//void buildAnimator(const XMLNode animationNode, Animator& out_animator);
	//void Parse(const char* animation, Animator& animator);

	void BuildAnimatior(const char* file, std::vector<ECS::Animation>& animator);
};
