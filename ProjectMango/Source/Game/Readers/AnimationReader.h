#pragma once


namespace AnimationReader
{
	void ReadAnimationData();
	void BuildAnimator(ECS::Entity entity, const char* file);

	bool AnimationExists(const char* animiation_id);

	// editor
	void Debug_GetAnimationIDs(std::vector<BasicString>& out_ids);
};

