#pragma once

#include "Graphics/Tileset.h"
#include "Animation.h"
#include "CharacterStates.h"

struct AnimationConfig;

#define FRAME_CAPTURING 0

class Animator
{
public:
	Animator();
	void AddAnimations(AnimationConfig* config);
	
	bool RunActive(float dt, bool force_loop = false);
	void ResetOnNewAnimation();

	const Animation* getAnimation(ActionState action) const;
	const Animation* getAnimation(ActionState action, VectorI direction, bool& flipped_direction) const;
	const Animation* activeAnimation() const { return mAnimationIndex != -1 ? &mAnimations[mAnimationIndex] : nullptr; }
	VectorF getAnimationSubRect() const;

	bool selectAnimation(ActionState state);
	bool selectAnimation(const Animation& anim);

	u32 getAnimationIndex(const Animation& anim);

	void start() { mState = TimeState::Running; }
	void pause() { mState = TimeState::Paused; }
	void stop();
	void restart();

	bool isRunning() const { return mState == TimeState::Running; }
	bool isStarted() const { return mState == TimeState::Running || mState == TimeState::Paused; }

	float FrameTime() const;
	VectorF FrameSize() const;

	ActionState activeAction() const { return mAnimations[mAnimationIndex].action; }
	Texture* activeSpriteSheet() const;

	const SpriteSheet& getSpritesheet(const Animation& animation) const;
	const SpriteSheet* getActiveSpritesheet() const;

	bool lastFrame() const { return mFrameIndex == mAnimations[mAnimationIndex].frameCount - 1; }
	bool canChange() const { return mLoops >= mAnimations[mAnimationIndex].minLoops; }
	bool finished() const { return mFinished; }

	std::vector<Animation> mAnimations;
	std::vector<SpriteSheet> mSpriteSheets;

#if FRAME_CAPTURING
	Queue<ActionState> mActions;
	u32 maxActionLength = 10;
	u32 frameCaptureIndex = 0;
#endif

	u32 mAnimationIndex;
	u32 mFrameIndex;
	
	TimeState mState;

	int mLoops;
	float mTime;
	bool mFlipped;
	bool mFinished; // only applies to non-looping animations
};
