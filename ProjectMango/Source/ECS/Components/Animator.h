#pragma once

enum class ActionState;
class Texture;
struct Config;

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
	
		SpriteSheet spriteSheet;

		// relative to the sprite
		VectorF entityColliderPos = VectorF(-1,-1);
		VectorF entityColliderSize = VectorF(-1,-1);

		VectorF entityColliderEndPos = c_invalidVector;

		// relative to the sprite
		VectorF attackColliderPos = VectorF(-1, -1);
		VectorF attackColliderSize = VectorF(-1, -1);

		// the about point of rotation
		VectorF objectCenter = VectorF(0.5f, 0.5f);

		int startIndex = 0;
		int frameCount = 0;

		int attackColliderFrameStart = 0;
		int attackColliderFrameEnd = 0;

		float frameTime = 0.0f;

		bool looping = true;
		bool reversing = false;
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

		void Init(const Config* config);

		void SetActiveSpriteFrame(Sprite& sprite);
		void StartAnimation(ActionState action);

		const Animation& GetActiveAnimation() const;
		const Animation* GetAnimation(ActionState action) const;

		bool OnLastFrame() const;

		bool IsValid() const;
	};
}