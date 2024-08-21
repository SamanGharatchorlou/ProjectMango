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

		// relative to the sprite
		VectorF colliderPos = VectorF(-1,-1);
		VectorF colliderSize = VectorF(-1,-1);

		// relative to size x
		float flipPointX;

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

		const Animation& GetActiveAnimation() const;
	};
}