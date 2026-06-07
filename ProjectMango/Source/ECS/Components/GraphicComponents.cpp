#include "pch.h"
#include "GraphicComponents.h"

#include "ECS/EntityCoordinator.h"
#include "Graphics/TextureManager.h"
#include "Graphics/STexture.h"
#include "Core/Helpers.h"
#include "Game/Readers/AnimationReader.h"
#include "GameComponents.h"
#include "Components.h"


namespace ECS
{
	// Sprite
	// ------------------------------------------------------------------
	void Sprite::Init(const char* sprite_id)
	{
		const Config* config = GetConfigFromEntity(entity);
		if(config)
		{
			if(!sprite_id)
			{
				sprite_id = config->data.GetString("sprite");
			}
		}

		image.id = sprite_id;
		SetTexture(image.id.c_str());
	}

	void Sprite::Init(const ECS::EntityMetaData& emd)
	{
		int render_layer = emd.data.GetInt("render_layer", (int)RenderLayer::BasicObject);
		render_layer = Maths::clamp(render_layer, 0, (int)RenderLayer::Top);

		params.renderLayer = (RenderLayer)render_layer;
		params.colourMod = emd.data.GetColour("colour");
		params.renderOffset = emd.data.GetVector("render_offset");

		const char* id = emd.data.GetString(kRequirement);
		if (!id)
			return;

		image.id = id;
		SetTexture(image.id.c_str());

		// no sprite yet, try get a coloured version
		Colour::Type colour_type = (Colour::Type)emd.data.GetInt("colour_type", -1);
		if (!image.texture && colour_type != -1)
		{
			StringBuffer64 coloured_sprite;
			AddColourPostfix(id, colour_type, coloured_sprite);

			image.id = coloured_sprite.c_str();
			SetTexture(image.id.c_str());
		}
	}

	void Sprite::Serialise(EntityMetaData& out_emd) const
	{
		if (image.texture)
		{
			out_emd.data.AddString(kRequirement, TextureManager::Get()->getTextureName(image.texture).c_str());
			out_emd.data.AddInt("render_layer", (int)params.renderLayer);
		}
	}

	void Sprite::SetTexture(const char* label)
	{
		image.id = label;
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
		frame.gridCount = frame_counts;
		Sprite* sprite = GetComponent(Sprite, entity);
		if(sprite && sprite->image.texture)
			frame.frameSize = sprite->image.texture->originalDimentions / frame_counts.toFloat();
	}

	void SpriteSheet::Init(const ECS::EntityMetaData& emd)
	{
		Sprite& sprite = GetOrAddComponent(Sprite, entity);

		frame.gridCount = emd.data.GetVector(kRequirement).toInt();
		frame.frameSize = sprite.image.texture->originalDimentions / frame.gridCount.toFloat();

		index = emd.data.GetInt("sprite_sheet_index", 0);
	}

	void SpriteSheet::Serialise(EntityMetaData& out_emd) const
	{
		out_emd.data.AddVectorF(kRequirement, frame.gridCount.toFloat() );
	}

	bool SpriteSheet::HasValidFrameIndex() const
	{
		return index >= 0 && index < (frame.gridCount.x * frame.gridCount.y);
	}
	
	RectF SpriteSheetFrame::GetFrameRect(int frame_index) const
	{
		VectorI index = IndexToGrid(frame_index, gridCount.x);
		VectorF top_left = frameSize * index.toFloat();
		return RectF( top_left, frameSize);
	}

	
	// Animator
	// ------------------------------------------------------------------
	void Animator::Init()
	{
		const char* id = GetName(entity);
		const char* animation = id;

		const Config* config = GetConfigFromEntity(entity);
		if(config)
		{
			animation = config->data.GetString("animation", id);
		}
		
		AnimationReader::BuildAnimator( entity, animation );

		activeAnimation = 0;
		state = TimeState::Running;

		if(config)
		{
			if (config->data.GetBool("randomise_frame_start") && IsValid())
			{
				int frame_start = (rand() % GetActiveAnimation()->frameCount) + 1;
				frameIndex = frame_start;
			}

			if (config->data.Contains("randomise_frame_speed"))
			{
				float variation = config->data.GetFloat("randomise_frame_speed");
				int var_range = (int)(variation * 100.0f);
				int value = rand() % (int)(var_range * 2);
				randomisedFrameTimeVariation = (float)(value - var_range) / 100.0f;
			}
		
			if (config->data.GetBool("randomise_animation", false))
			{
				activeAnimation = Maths::randomNumberBetween(0, (int)animations->size());
			}
		}

		if(!IsValid())
			DebugPrint(Log, "Entity %s has invalid animaton", id );
	}

	void Animator::Init(const EntityMetaData& emd)
	{
		AnimationReader::BuildAnimator( entity, emd.data.GetString(kRequirement) );		
		
		activeAnimation = 0;
		state = TimeState::Running;

		if (!IsValid())
		{
			DebugPrint(Log, "Entity %s has invalid animaton", GetName(entity));
			return;
		}

		if (emd.data.Contains("randomise_frame_start") && IsValid())
		{
			int frame_start = (rand() % GetActiveAnimation()->frameCount) + 1;
			frameIndex = frame_start;
		}
		
		if (emd.data.Contains("randomise_frame_speed"))
		{
			float variation = emd.data.GetFloat("randomise_frame_speed");
			int var_range = (int)(variation * 100.0f);
			int value = rand() % (int)(var_range * 2);
			randomisedFrameTimeVariation = (float)(value - var_range) / 100.0f;
		}

		if (emd.data.Contains("randomise_animation"))
		{
			activeAnimation = Maths::randomNumberBetween(0, (int)animations->size());
		}

		Sprite& sprite = GetOrAddComponent(Sprite, entity);
		sprite.Init(emd);

		EntityState& es = GetOrAddComponent(EntityState, entity);
	}

	bool Animator::IsValid() const
	{
		return animations && activeAnimation >= 0 && activeAnimation < animations->size();
	}

	RectF Animator::GetActiveSubRect() const
	{
		if(!IsValid())
			return InvalidRectF;

		const Animation& animation = (*animations)[activeAnimation];
		int active_frame_index = animation.startIndex + frameIndex;
		if(animation.reversing)
		{
			active_frame_index = animation.startIndex + animation.frameCount - (frameIndex + 1);
		}

		return animation.frame.GetFrameRect(active_frame_index);
	}

	void Animator::StartAnimation(Action::Enum action)
	{
		if(animations)
		{
			for( u32 i = 0; i < animations->size(); i++ )
			{
				if( (*animations)[i].action == action)
				{
					state = TimeState::Running;

					activeAnimation = i;
					frameIndex = 0;
					loopCount = 0;
					timer = 0;

					return;
				}
			}
		}

		DebugPrint(Warning, "No animation found for action %s", ActionToString(action));
	}		
	const Animation* Animator::GetActiveAnimation() const
	{
		if(IsValid())
		{
			return &(*animations)[activeAnimation];
		}
		
		return nullptr;
	}

	const Animation* Animator::GetAnimation(Action::Enum action) const
	{
		for( u32 i = 0; i < animations->size(); i++ )
		{
			if( (*animations)[i].action == action)
			{
				return &(*animations)[i];
			}
		}

		return nullptr;
	}

	
	bool Animator::HasAnimation(Action::Enum action) const
	{
		for( u32 i = 0; animations && i < animations->size(); i++ )
		{
			if( (*animations)[i].action == action)
			{
				return true;
			}
		}

		return false;
	}

	bool Animator::OnLastFrame() const
	{
		return frameIndex == (*animations)[activeAnimation].frameCount - 1;
	}

	bool operator == (const Animation& a, const Animation& b)
	{
		return a.image.texture == b.image.texture && 
			a.startIndex == b.startIndex &&
			a.frameCount == b.frameCount &&
			a.reversing == b.reversing;
	}
}