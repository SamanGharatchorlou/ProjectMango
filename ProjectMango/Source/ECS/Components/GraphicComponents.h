#pragma once

#include "ComponentHelpers.h"

class STexture;

// all components related to graphics and rendering go here
namespace ECS
{
	struct SpriteParameters
	{
		SColour colourMod;
		SDL_RendererFlip flip = SDL_FLIP_NONE;
		
		// in degress (because of the render function input)
		float rotation = 0; 
		RenderLayer renderLayer = RenderLayer::None;

		bool disabled = false;
		bool canFlip = true;
	};

	struct SpriteImage
	{
		BasicString id;
		STexture* texture = nullptr;
	};

	struct Sprite
	{
		COMPONENT_TYPE(Sprite)

		SpriteImage image;
		SpriteParameters params;
		
		void Init(const char* sprite_id);
		bool IsFlipped() const { return params.flip == SDL_FLIP_HORIZONTAL; }
		bool IsValid() const;

		void SetTexture(const char* label);
	};

	struct SpriteSheetFrame
	{
		VectorF size;
		VectorI counts;

		RectF GetFrameRect(int index) const;
	};

	struct SpriteSheet
	{
		COMPONENT_TYPE(SpriteSheet)
			
		SpriteSheetFrame frame;
		int index = 0;

		void Init(VectorI frame_counts);
		bool HasValidFrameIndex() const;
	};
	
	struct LayeredSprite
	{
		COMPONENT_TYPE(LayeredSprite)

		struct Layer
		{
			Sprite sprite;
			RectF rect;
		};

		std::vector<Layer> spriteLayers;
	};

	struct SpriteCycle
	{
		COMPONENT_TYPE(SpriteCycle)

		BasicString spritePrefix;
		int index = 0;
	};

	struct Animation
	{
		SpriteImage image;
		SpriteSheetFrame frame;

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

		std::vector<Animation> animations;

		int activeAnimation = 0;
		int frameIndex = 0;

		TimeState state = TimeState::Stopped;

		int loopCount = 0;
		float timer = 0.0f;

		void Init();

		void StartAnimation(Action::Enum action);

		const Animation& GetActiveAnimation() const;
		const Animation* GetAnimation(Action::Enum action) const;
		RectF GetActiveSubRect() const;

		bool OnLastFrame() const;

		bool IsValid() const;
	};

	//struct VFX
	//{
	//	COMPONENT_TYPE(VFX)

	//	Animation animation;

	//};
}