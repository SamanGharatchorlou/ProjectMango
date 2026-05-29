#pragma once

#include "ComponentHelpers.h"

struct STexture;

// all components related to graphics and rendering go here
namespace ECS
{
	struct SpriteParameters
	{
		VectorF renderOffset;

		SColour colourMod;
		SDL_RendererFlip flip = SDL_FLIP_NONE;
		
		// in degress (because of the render function input)
		float rotation = 0; 
		RenderLayer renderLayer = RenderLayer::None;

		bool disabled = false;
	};

	struct SpriteImage
	{
		BasicString id;
		STexture* texture = nullptr;
	};

	struct Sprite
	{
		COMPONENT_TYPE(Sprite)

		static constexpr const char* kRequirement = "sprite";

		SpriteImage image;
		SpriteParameters params;

		void Init(const EntityMetaData& emd);
		void Init(const char* sprite_id);
		void Serialise(EntityMetaData& out_emd) const;

		bool IsValid() const;

		void SetTexture(const char* label);
	};

	struct SpriteSheetFrame
	{
		VectorF frameSize;
		VectorI gridCount;

		RectF GetFrameRect(int index) const;
	};

	struct SpriteSheet
	{
		COMPONENT_TYPE(SpriteSheet)

		static constexpr const char* kRequirement = "sprite_sheet_frames";
			
		SpriteSheetFrame frame;
		int index = 0;

		void Init(const EntityMetaData& emd);
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
		
	bool operator == (const Animation& a, const Animation& b);

	struct Animator
	{
		COMPONENT_TYPE(Animator)

		static constexpr const char* kRequirement = "animator";

		const std::vector<Animation>* animations = nullptr;

		int activeAnimation = 0;
		int frameIndex = 0;

		TimeState state = TimeState::Stopped;

		int loopCount = 0;
		float timer = 0.0f;

		float randomisedFrameTimeVariation = 0.0f;

		void Init();
		void Init(const EntityMetaData& emd);

		void StartAnimation(Action::Enum action);

		const Animation* GetActiveAnimation() const;
		const Animation* GetAnimation(Action::Enum action) const;
		bool HasAnimation(Action::Enum action) const;
		RectF GetActiveSubRect() const;

		bool OnLastFrame() const;

		bool IsValid() const;
	};

	struct Jiggler
	{
		COMPONENT_TYPE(Jiggler)

		u64 startTime = 0;

		// amount it moves
		float amplitude = 0.0f;
		// how fast it moves
		float frequency = 0.0f;

		// reduces amplitude over time
		u64 decayStartTime = 0;
		float decayTime = 0.0f;

		// number of loops before the decay kicks in
		int undisturbedLoops = 0;

		//float GetXOffset() const;
	};
}