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
		void InitCollider(Collider& collider);

		void SetLocalPosition(VectorF pos);
		void SetWorldPosition(VectorF pos);
		void SetWorldRect(const VectorF& pos, const VectorF& size);

		void SetObjectCenter(VectorF pos);
		RectF GetObjectRect() const;

		VectorF GetObjectCenter() const;
		RectF GetRect() const;

		VectorF GetRelativePosition(VectorF relative) const;

		static VectorF GetObjectCenter(ECS::Entity entity);
	};

	struct Sprite
	{
		COMPONENT_TYPE(Sprite)

		Sprite();

		BasicString ID;
		BasicString debugID;

		RectF subRect;
		STexture* texture;
		
		SColour colourMod;

		VectorF flipPoint;
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

		bool mustFinishAnimation = false;;
	};

	struct Target
	{
		COMPONENT_TYPE(Target)

		Entity target = EntityInvalid;

		static Entity GetPlayer();
		static Entity GetEnemy();

		static Entity GetValidTarget(Entity entity);
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

		DeathScentence();

		// begin this animator entity on death
		Entity startAnimatiorOnDeath;

		// kill at animator loop count
		int deathLoops;
		// kill on timer
		float deathTimer;
		// kill once in area
		RectF deathZone;
		
		bool canDie = false;

		void Update(float dt);
		void OnDeath();
	};

	struct Callback
	{
		COMPONENT_TYPE(Callback)

		BasicString callback;
	};
	
	//typedef Entity (*EntitySpawnFn)( const ECS::EntityMetaData& );

	struct Spawner
	{
		COMPONENT_TYPE(Spawner)

		Spawner();

		Entity spawnedEntity;
		Entity spawnRequest;

		//bool IsSpawning() { return entityToSpawn != EntityInvalid; }

		//bool Spawn( Entity entity );
		//void Update();
	};

	struct SpawnRequest
	{
		COMPONENT_TYPE(SpawnRequest)

		ECS::EntityMetaData emd;
		int frameTime;
	};

	struct Door
	{
		COMPONENT_TYPE(Door)

		Door();
			
		// top and bottom
		Entity colliders[2];

		float triggerRange;

		void Init();
		void Update();

		void GenerateColliders(float width);
	};
}