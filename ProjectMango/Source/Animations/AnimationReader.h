#pragma once

class Animator;

namespace AnimationReader
{
	//void buildAnimator(const XMLNode animationNode, Animator& out_animator);
	void Parse(const char* animation, Animator& animator);
};
