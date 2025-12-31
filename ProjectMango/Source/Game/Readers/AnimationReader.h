#pragma once


namespace AnimationReader
{
	void ReadAnimationData();
	void BuildAnimator(ECS::Entity entity, const char* file);

	// editor
	void Debug_GetAnimationIDs(std::vector<BasicString>& out_ids);
};

