#include "pch.h"
#include "GraphicComponents.h"

#include "ECS/EntityCoordinator.h"
#include "Graphics/TextureManager.h"
#include "Graphics/STexture.h"
#include "Core/Helpers.h"
#include "Game/Readers/AnimationReader.h"


namespace ECS
{
	// Sprite
	// ------------------------------------------------------------------
	void Sprite::Init(const char* sprite_id)
	{
		if(!sprite_id)
		{
			if(const Config* config = GetConfigFromEntity(entity))
			{
				sprite_id = config->data.GetString("sprite");
			}
		}

		image.id = sprite_id;
		SetTexture(image.id.c_str());
	}

	void Sprite::SetTexture(const char* label)
	{
		image.texture = TextureManager::Get()->getTexture(label, FileManager::Folder::Images);
	}

	bool Sprite::IsValid() const
	{
		return image.texture && params.renderLayer != RenderLayer::None && !params.disabled;
	}

	// SpriteSheet
	// ------------------------------------------------------------------
	void SpriteSheet::Init(VectorI frame_counts)
	{
		frame.counts = frame_counts;
		Sprite* sprite = GetComponent(Sprite, entity);
		if(sprite && sprite->image.texture)
			frame.size = sprite->image.texture->originalDimentions / frame_counts.toFloat();
	}

	bool SpriteSheet::HasValidFrameIndex() const
	{
		return index >= 0 && index < (frame.counts.x * frame.counts.y);
	}
	
	RectF SpriteSheetFrame::GetFrameRect(int frame_index) const
	{
		VectorI index = IndexToGrid(frame_index, counts.x);
		VectorF top_left = size * index.toFloat();
		return RectF( top_left, size);
	}

	
	// Animator
	// ------------------------------------------------------------------
	void Animator::Init()
	{
		const Config* config = GetConfigFromEntity(entity);
		if(!config)
			return;

		const char* animation = config->data.GetString("animation");
		AnimationReader::BuildAnimatior( entity, animation );
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
		
		if (config->data.GetBool("randomise_animation", false))
		{
			activeAnimation = Maths::randomNumberBetween(0, (int)animations.size());
		}
	}

	bool Animator::IsValid() const
	{
		return animations.size() > 0;
	}

	RectF Animator::GetActiveSubRect() const
	{
		if(!IsValid())
			return InvalidRectF;

		const Animation& animation = animations[activeAnimation];
		int active_frame_index = animation.startIndex + frameIndex;
		if(animation.reversing)
		{
			active_frame_index = animation.startIndex + animation.frameCount - (frameIndex + 1);
		}

		return animation.frame.GetFrameRect(active_frame_index);
	}

	void Animator::StartAnimation(Action::Enum action)
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

	const Animation* Animator::GetAnimation(Action::Enum action) const
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