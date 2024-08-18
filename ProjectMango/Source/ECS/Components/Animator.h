#pragma once

enum class ActionState;
class Texture;


namespace ECS
{
	struct Sprite;

	struct SpriteSheet
	{
		BasicString ID;
		Texture* texture;
		VectorF frameSize;
		VectorI sheetSize;
	};

	struct Animation
	{
		ActionState action = (ActionState)0;
	
		SpriteSheet* spriteSheet;

		int startIndex = 0;
		int frameCount = 0;

		float frameTime = 0.0f;

		bool looping = true;
	};

	struct Animator
	{
		COMPONENT_TYPE(Animator)

		std::vector<Animation> animations;

		u32 activeAnimation;
		u32 frameIndex;

		TimeState state;

		int loopCount;
		float timer;

		void SetActiveSpriteFrame(Sprite& sprite);
		void StartAnimation(ActionState action);
	};
}