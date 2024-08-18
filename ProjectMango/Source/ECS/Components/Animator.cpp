#include "pch.h"
#include "Animator.h"

#include "Components.h"

namespace ECS
{
	void Animator::SetActiveSpriteFrame(Sprite& sprite)
	{
		if(animations.size() == 0)
			return;

		const Animation& animation = animations[activeAnimation];

		sprite.texture = animation.spriteSheet->texture;

		int active_frame_index = animation.startIndex + frameIndex;

		float x_frames = (float)animation.spriteSheet->sheetSize.x;

		int index_y = (int)((float)active_frame_index / x_frames);
		int index_x = (int)(active_frame_index - (index_y * x_frames));

		const VectorF frame_size = animation.spriteSheet->frameSize;
		VectorF top_left = frame_size * VectorI(index_x, index_y).toFloat();
		sprite.subRect = RectF( top_left, frame_size);
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

		DebugPrint(Warning, "No animation found for action %s", actionToString(action));
	}
}