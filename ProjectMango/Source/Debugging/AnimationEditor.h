#pragma once

namespace AnimationEditor
{
	void DoEditor();

	struct AnimationState
	{
		StringBuffer64 selectedAnimation;
		VectorI frameCounts = VectorI(17,14);

		std::vector<VectorF> selectedFrames;
		std::vector<VectorI> selectedFrameIndexes;

		int targetFrame = 0;
		float frameTime = 0.1f;
		float frameTimer = 0.0f;
		bool isPlayingFrames = true;

		bool showFrameSplit = true;

		void HandleInput();
		void Render();
	};

	AnimationState& GetAnimationState();
}