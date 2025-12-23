#pragma once

//#include "Core/stack.h"
#include "ComponentHelpers.h"

class STexture;

// when adding a component, define it in EntityCommon.h
// then setup how its updated in ComponentsSetup

// more generic components go here
namespace ECS
{
	struct Collider;

	struct EntityData
	{
		COMPONENT_TYPE(EntityData)
		
		EntityData();

		BasicString id;

		ECS::Entity parent;
		std::vector<Entity> children;

		static void SetParent(Entity entity, Entity parent);
	};

	struct Transform
	{
		COMPONENT_TYPE(Transform)

		Transform();

		// top left
		VectorF targetWorldPosition;
		VectorF worldPosition;
		VectorF localPosition;

		VectorF renderOffset;
		VectorF size;

		// set through the anim config, might not be the technical center, 
		// but it should be the visual one (relative value)
		VectorF center;

		bool ignoreOutOfBounds;
		
		void Init(const EntityMetaData* emd, Collider& collider);
		void Init(const EntityMetaData* emd);

		void SetLocalPosition(VectorF pos);
		void SetWorldPosition(VectorF pos);
		void SetWorldRect(const VectorF& pos, const VectorF& size);

		void SetObjectCenter(VectorF pos);
		RectF GetObjectRect() const;

		VectorF GetHorizontalFlipPoint() const;
		//VectorF GetRelativeObjectCenter() const;
		VectorF GetObjectCenter() const;
		RectF GetRect() const;

		VectorF GetRelativePosition(VectorF relative) const;

		static VectorF GetObjectCenter(ECS::Entity entity);
	};

	struct Sprite
	{
		COMPONENT_TYPE(Sprite)

		Sprite();

		BasicString Id;
		BasicString debugID;

		RectF subRect;
		STexture* texture;
		
		SColour colourMod;
		SDL_RendererFlip flip;
		
		// in degress (because of the render function input)
		float rotation; 
		RenderLayer renderLayer;

		bool disabled;
		bool canFlip;
		
		void Init();
		bool IsFlipped() const { return flip == SDL_FLIP_HORIZONTAL; }

		void SetTexture(const char* label);
	};

	// simple sprite sheet, 1 row
	struct SpriteSheet
	{
		COMPONENT_TYPE(SpriteSheet)

		BasicString Id;
		STexture* texture;

		int count = 0;
		int index = 0;

		VectorF frameSize;
		RenderLayer renderLayer;
		SColour colourMod;

		void Init(const char* sprite_sheet, int count);
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

	struct Audio
	{
		COMPONENT_TYPE(Audio)

		static constexpr const char* c_noId = "";

		struct Group
		{
			std::vector<BasicString> sounds;
			int time;
		};

		std::unordered_map<BasicString, Group> soundEffects;

		void Play(const char* sound_effect = c_noId);

		// a group has a format like "GunShot 1", then each audio within that group will be
		// GunShot 1-1, GunShot 1-2, etc. starting at 1 and ending until there is not another sequential number
		void PopulateGroup(const char* group, int time = -1, const char* id = c_noId);
	};

	struct EntityState
	{
		COMPONENT_TYPE(EntityState)

		Action::Enum next = Action::None;
		Action::Enum current = Action::None;
		std::vector<Action::Enum> backlog;

		bool justChanged;
		//float timeInState;

		//bool mustFinishAnimation = false;;
	};

	struct Target
	{
		COMPONENT_TYPE(Target)

		Entity targetEntity = EntityInvalid;
		bool isEnemy = false;
		bool isPlayer = false;

		Entity GetTarget() const;
		static Entity GetTarget(Entity entity);

		static Entity GetPlayer();
		static Entity GetEnemy();
	};

	struct PlayerController // more like a tag "I am a player"
	{
		COMPONENT_TYPE(PlayerController)
	};


	struct Damage
	{
		COMPONENT_TYPE(Damage)

		Damage();

		// which entity we've already applied to, dont apply again
		std::vector<Entity> appliedTo;

		// may apply a force
		VectorF source;
		float force;

		// the damage
		float value;

		void Init();
		bool CanApplyTo(Entity entity) const;
		void ApplyTo(Entity entity);
	};

	struct Health
	{
		COMPONENT_TYPE(Health)

		Health();

		float maxHealth;
		float currentHealth;

		bool invulnerable;
		
		void Init();
		void ApplyDamage(float damage);
	};


	struct DeathScentence
	{
		COMPONENT_TYPE(DeathScentence)

		// kill at animator loop count
		int deathLoops = -1;

		// kill on timer
		float deathTimer = -FLT_MAX;
		
		void Update(float dt);
		bool CanDie();
	};

	struct Callback
	{
		COMPONENT_TYPE(Callback)

		BasicString callback;
	};
	
	struct Spawner
	{
		COMPONENT_TYPE(Spawner)

		Entity spawnedEntity = EntityInvalid;
		Entity spawnRequest = EntityInvalid;
	};

	struct SpawnRequest
	{
		COMPONENT_TYPE(SpawnRequest)

		EntityMetaData emd;
		Entity owner = EntityInvalid;
		int frameTime = 0;
		int cardRegistryIndex = -1;
	};

}