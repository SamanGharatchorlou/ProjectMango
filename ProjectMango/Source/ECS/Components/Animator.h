#pragma once

#include "ECS/Components/Components.h"

namespace ECS
{
	struct SpriteSheet
	{
		BasicString ID;
		STexture* texture;
		VectorF frameSize;
		VectorI sheetSize;
	};

	struct Animation
	{
		SpriteSheet spriteSheet;

		// relative to the sprite
		VectorF entityColliderPos = VectorF(-1,-1);
		VectorF entityColliderSize = VectorF(-1,-1);

		// relative to the sprite
		VectorF attackColliderPos = VectorF(-1, -1);
		VectorF attackColliderSize = VectorF(-1, -1);

		// the about point of rotation
		VectorF objectCenter = VectorF(0.5f, 0.5f);
		
		Action::Enum action = Action::None;

		int startIndex = 0;
		int frameCount = 0;

		float frameTime = 0.0f;

		bool looping = true;
		bool reversing = false;
	};

	struct Animator
	{
		COMPONENT_TYPE(Animator)
		Animator();

		std::vector<Animation> animations;

		u32 activeAnimation;
		u32 frameIndex;

		TimeState state;

		int loopCount;
		float timer;

		void Init();

		// can remove
		void SetActiveSpriteFrame(Sprite& sprite);
		void StartAnimation(Action::Enum action);

		const Animation& GetActiveAnimation() const;
		const Animation* GetAnimation(Action::Enum action) const;

		bool OnLastFrame() const;

		bool IsValid() const;
	};
}