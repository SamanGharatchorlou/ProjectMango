#pragma once


namespace AnimationReader
{
	void ReadAnimationData();
	void ClearAnimationData();

	void BuildAnimator(ECS::Entity entity, const char* file);

	bool AnimationExists(const char* animiation_id);
	VectorF GetAnimationFrameSize(const char* animiation_id);

	// editor
	void Debug_GetAnimationIDs(std::vector<BasicString>& out_ids);
};

