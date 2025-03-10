#include "pch.h"
#include "Animator.h"

#include "ECS/EntityCoordinator.h"
#include "Components.h"
#include "Core/Helpers.h"
#include "Animations/CharacterStates.h"
#include "Animations/AnimationReader.h"
#include "System/Files/Config.h"

namespace ECS
{
	Animator::Animator() : 
		activeAnimation(0), 
		frameIndex(0), 
		state(TimeState::Stopped), 
		loopCount(0), 
		timer(0) 
	{ }
	
	void Animator::Init(const Config* config)
	{
		const char* animation = config->data.GetString("animation");
		AnimationReader::BuildAnimatior( *this, animation);
		activeAnimation = 0;
		state = TimeState::Running;

		if (config->data.GetBool("randomise_frame_start"))
		{
			int frame_start = (rand() % GetActiveAnimation().frameCount) + 1;
			frameIndex = frame_start;
		}

		if (config->data.Contains("randomise_frame_speed"))
		{
			float variation = config->data.GetFloat("randomise_frame_speed");

			int var_range = (int)(variation * 100.0f);

			int value = rand() % (int)(var_range * 2);
			float diff = (float)(value - var_range) / 100.0f;

			ECS::Animation& animation = animations[activeAnimation];
			animation.frameTime = animation.frameTime + (diff * animation.frameTime);
		}
	}

	bool Animator::IsValid() const
	{
		return animations.size() > 0;
	}

	void Animator::SetActiveSpriteFrame(Sprite& sprite)
	{
		if(!IsValid())
			return;

		const Animation& animation = animations[activeAnimation];

		sprite.texture = animation.spriteSheet.texture;

		int active_frame_index = animation.startIndex + frameIndex;

		if(animation.reversing)
		{
			active_frame_index = animation.startIndex + animation.frameCount - (frameIndex + 1);
		}

		int x_frames = animation.spriteSheet.sheetSize.x;
		VectorI index = IndexToGrid(active_frame_index, x_frames);

		const VectorF frame_size = animation.spriteSheet.frameSize;
		VectorF top_left = frame_size * index.toFloat();
		sprite.subRect = RectF( top_left, frame_size);

		sprite.flipPoint = animation.objectCenter;
	}

	void Animator::StartAnimation(ActionState action)
	{
		for( u32 i = 0; i < animations.size(); i++ )
		{
			if(animations[i].action == action)
			{
				state = TimeState::Running;

				activeAnimation = i;
				frameIndex = 0;
				loopCount = 0;
				timer = 0;

				return;
			}
		}

		DebugPrint(Warning, "No animation found for action %s", ActionToString(action));
	}

	const Animation& Animator::GetActiveAnimation() const
	{
		ASSERT(IsValid(), "Invalid animator, cannot get active animation");
		return animations[activeAnimation];
	}

	const Animation* Animator::GetAnimation(ActionState action) const
	{
		for( u32 i = 0; i < animations.size(); i++ )
		{
			if(animations[i].action == action)
			{
				return &animations[i];
			}
		}

		return nullptr;
	}

	bool Animator::OnLastFrame() const
	{
		return frameIndex == animations[activeAnimation].frameCount - 1;
	}
}