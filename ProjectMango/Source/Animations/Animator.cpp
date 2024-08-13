#include "pch.h"
#include "Animator.h"

Animator::Animator() :
	mAnimationIndex(-1), 
	mFrameIndex(0),
	mTime(0.0f),
	mLoops(0),
	mState(TimeState::Stopped) { }

VectorF Animator::getAnimationSubRect() const
{
	const Animation* animation =  activeAnimation();
	if(!animation)
		return VectorF::zero();

	int index = animation->startIndex + mFrameIndex;
	const SpriteSheet& ss = getSpritesheet(*animation);
	VectorI bounaries = ss.boundaries;

	int y = 0;
	int x = 0;
	while (index >= bounaries.x)
	{
		y++;
		index -= bounaries.x;
	}
	x = index;

	return ss.frameSize * Vector2D<int>(x, y).toFloat();
}


void Animator::ResetOnNewAnimation()
{
	mTime = 0.0f;
	mFrameIndex = 0;
	mLoops = 0;
	mFlipped = false;
	mFinished = false;
}

bool Animator::selectAnimation(ActionState action)
{
	for (int i = 0; i < mAnimations.size(); i++)
	{
		if (mAnimations[i].action == action)
		{
#if FRAME_CAPTURING
			mActions.push(action);
			if (mActions.size() > maxActionLength)
				mActions.popFront();
#endif

			//DebugPrint(PriorityLevel::Log, "Selecting animation %s (%d,%d)", actionToString(action).c_str(), mAnimations[i].direction.x, mAnimations[i].direction.y);
			mAnimationIndex = i;
			ResetOnNewAnimation();
			return true;
		}
	}

	DebugPrint(Warning, "Animation state %d not present in animator", (int)action);
	return false;
}

bool Animator::selectAnimation(const Animation& anim)
{
	for (int i = 0; i < mAnimations.size(); i++)
	{
		if (mAnimations[i].action == anim.action)
		{
			const bool correct_direction = mAnimations[i].direction == anim.direction;
			const bool flipped_direction = mAnimations[i].canFlip && mAnimations[i].direction == anim.direction * -1;

			if(correct_direction || flipped_direction)
			{
	#if FRAME_CAPTURING
				mActions.push(action);
				if (mActions.size() > maxActionLength)
					mActions.popFront();
	#endif
			
				//DebugPrint(PriorityLevel::Log, "Selecting animation %s (%d,%d)", actionToString(anim.action).c_str(), mAnimations[i].direction.x, mAnimations[i].direction.y);
				mAnimationIndex = i;
				ResetOnNewAnimation();
				mFlipped = flipped_direction;
				return true;
			}
		}
	}

	// fallback
	return selectAnimation(anim.action);
}

const Animation* Animator::getAnimation(ActionState action) const
{
	for (int i = 0; i < mAnimations.size(); i++)
	{
		if (mAnimations[i].action == action)
		{
			return &mAnimations[i];
		}
	}

	DebugPrint(Warning, "No animation with action %d", action);
	return nullptr;	
}

const Animation* Animator::getAnimation(ActionState action, VectorI direction, bool& flipped_direction) const
{
	for (int i = 0; i < mAnimations.size(); i++)
	{
		if (mAnimations[i].action == action)
		{
			const bool correct_direction = mAnimations[i].direction == direction;
			flipped_direction = mAnimations[i].canFlip && mAnimations[i].direction == direction * -1;

			if(correct_direction || flipped_direction)
				return &mAnimations[i]; 
		}
	}

	return getAnimation(action);
}

u32 Animator::getAnimationIndex(const Animation& anim)
{
	for (u32 i = 0; i < mAnimations.size(); i++)
	{
		if (mAnimations[i].action == anim.action)
		{
			const bool correct_direction = mAnimations[i].direction == anim.direction;
			const bool flipped_direction = mAnimations[i].canFlip && mAnimations[i].direction == anim.direction * -1;

			if(correct_direction || flipped_direction)
			{
				return i;
			}
		}
	}

	return -1;
}

void Animator::stop()
{
	mTime = 0.0f;
	mState = TimeState::Stopped;
}

void Animator::restart()
{
	ResetOnNewAnimation();
}

bool Animator::RunActive(float dt, bool force_loop)
{
	if (mState == TimeState::Running)
		mTime += dt;

	const Animation* animation = activeAnimation();
	if(!animation)
		return false;

	bool progress_frame = mTime >= animation->frameTime;
	int next_frame = mFrameIndex + 1;

	if (progress_frame)
	{	
		// cant pass last frame if we can loop
		if(!animation->looping && lastFrame() && !force_loop)
		{
			mFinished = true;
			return false;
		}

		mFrameIndex = next_frame % animation->frameCount;
		mTime = 0.0f;

		if (mFrameIndex == 0)
		{
			mLoops++;
			return true;
		}
	}

	return false;
}

VectorF Animator::FrameSize() const 
{ 
	const Animation* animation = activeAnimation();
	if(!animation)
		return VectorF::zero();

	return getSpritesheet(*animation).frameSize; 
}

float Animator::FrameTime() const 
{ 
	const Animation* animation = activeAnimation();
	if(!animation)
		return 0.0f;

	return animation->frameTime; 
}

Texture* Animator::activeSpriteSheet() const 
{
	int sprite_sheet_index = mAnimations[mAnimationIndex].spriteSheetIndex;
	return mSpriteSheets[sprite_sheet_index].texture; 
}

const SpriteSheet& Animator::getSpritesheet(const Animation& animation) const
{
	return mSpriteSheets[animation.spriteSheetIndex]; 
}

const SpriteSheet* Animator::getActiveSpritesheet() const
{
	if(mAnimationIndex >= 0 && mAnimationIndex < mAnimations.size())
	{
		int sprite_sheet_index = mAnimations[mAnimationIndex].spriteSheetIndex;
		return &mSpriteSheets[sprite_sheet_index];
	}

	return nullptr;
}