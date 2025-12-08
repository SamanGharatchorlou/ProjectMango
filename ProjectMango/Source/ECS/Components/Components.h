#pragma once

#include "Core/stack.h"
#include "Entities/States/CharacterAction.h"

class STexture;
struct Config;

// when adding a component, define it in EntityCommon.h
// then setup how its updated in ComponentsSetup

namespace ECS
{
	struct Collider;

	enum Direction { Up, Right, Down, Left, Count };

	static const VectorI s_directions[Direction::Count] 
	{ 
		VectorI(0,-1), VectorI( 1, 0), VectorI(0, 1), VectorI(-1, 0)
	};

	struct EntityData
	{
		COMPONENT_TYPE(EntityData)

		BasicString id;

		// might be a general name like, "Rune" then use the id to get
		BasicString subType;

		ECS::Entity parent;
		std::vector<Entity> children;

		static void SetParent(Entity entity, Entity parent);
	};

	struct Transform
	{
		COMPONENT_TYPE(Transform)

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
		
		void Init(const Config* config, VectorF pos, Collider& collider);
		void Init(const Config* config, VectorF pos);
		void InitCollider(Collider& collider);

		void SetLocalPosition(VectorF pos);
		void SetWorldPosition(VectorF pos);
		static void SetWorldPosition(ECS::Entity entity, VectorF pos);
		void SetWorldRect(const VectorF& pos, const VectorF& size);

		void SetObjectCenter(VectorF pos);
		VectorF GetObjectCenter() const;
		RectF GetRect() const;

		VectorF GetRelativePosition(VectorF relative) const;

		static VectorF GetObjectCenter(ECS::Entity entity);
	};

	enum class RenderLayer
	{
		None = -1,
		Bottom = 0,
		Scenery = 1,
		Spell = 4,
		Characters = 5,
		BasicObject = 6,
		UI = 8,
		Top = 9,
		Count = 10
	};

	struct Sprite
	{
		COMPONENT_TYPE(Sprite)

		RectF subRect;
		STexture* texture;
		
		SColour colourMod;

		VectorF flipPoint;
		SDL_RendererFlip flip;
		bool canFlip;
		
		// in degress (because of the render function input)
		float rotation; 
		RenderLayer renderLayer;
		
		void Init(const Config* config);
		bool IsFlipped() const { return flip == SDL_FLIP_HORIZONTAL; }
		void SetTexture(const char* label);
	};
	
	static constexpr const char* c_noAudioId = "";

	struct Audio
	{
		COMPONENT_TYPE(Audio)

		struct Group
		{
			std::vector<BasicString> sounds;
			int time;
		};

		std::unordered_map<const char*, Group> soundEffects;

		void Play(const char* sound_effect = c_noAudioId);

		// a group has a format like "GunShot 1", then each audio within that group will be
		// GunShot 1-1, GunShot 1-2, etc. starting at 1 and ending until there is not another sequential number
		void PopulateGroup(const char* group, int time = -1, const char* id = c_noAudioId);
	};

	struct CharacterState
	{
		COMPONENT_TYPE(CharacterState)

		ActionStack<CharacterAction> actions;

		// overload common functions
		Character* character;

		VectorI movementInput;

		bool isRanged;
		bool isMelee;

		// melee only (split this into differnt state parts?)
		bool canEnterHover;
		
		void Init(const Config* config);
	};

	struct PlayerController // more like a tag "I am a player"
	{
		COMPONENT_TYPE(PlayerController)
	};

	struct Pathing
	{
		COMPONENT_TYPE(Pathing)

		//Entity target;

		// the incremental next position to move to: pos + speed
		// probably set by the AIController
		//VectorF currentLocation;
		VectorF targetLocation;

		// sets the bounds
		u32 levelIndex = -1;

		bool hasValidPath = false;

		void Init();
	};

	struct Damage
	{
		COMPONENT_TYPE(Damage)

		// which entity we've already applied to, dont apply again
		std::vector<Entity> appliedTo;

		// may apply a force
		VectorF source;
		float force;

		// the damage
		float value;

		void Init(const Config* config);
		bool CanApplyTo(Entity entity) const;
		void ApplyTo(Entity entity);
	};

	struct Health
	{
		COMPONENT_TYPE(Health)

		float maxHealth;
		float currentHealth;

		bool invulnerable;
		
		void Init(const Config* config);
		void ApplyDamage(const Damage& damage);
	};

	struct DeathScentence
	{
		COMPONENT_TYPE(DeathScentence)

		// begin this animator entity on death
		Entity startAnimatiorOnDeath;

		// kill at animator loop count
		int deathLoops;
		// kill on timer
		float deathTimer;
		// kill once in area
		RectF deathZone;
		
		void Update(float dt);
		void OnDeath();
	};
	
	typedef Entity (*EntitySpawnFn)( const ECS::EntityMetaData& );

	struct Spawner
	{
		COMPONENT_TYPE(Spawner)

		EntitySpawnFn entitySpawnFn;
		const char* spawnId;

		bool IsSpawning() { return entitySpawnFn != nullptr; }

		bool Spawn(const char* spawn_id, EntitySpawnFn spawnFn);
		void Update();
	};

	struct Door
	{
		COMPONENT_TYPE(Door)
			
		// top and bottom
		Entity colliders[2];

		float triggerRange;

		void Init();
		void Update();

		void GenerateColliders(float width);
	};

	struct Pickup
	{
		COMPONENT_TYPE(Pickup)

		BasicString itemId;
		// not just IdConfig, since Id is probably just Rune
		// but the config would be ReboundRuneConfig
		BasicString config;
		//OnPickupFn onPickupFn;

		bool pickedUp;

		void Update();
	};

	// should be attached to a character, will rotate with the cursor
	struct Arm
	{
		COMPONENT_TYPE(Arm)
			
		Entity target;
		VectorF anchorPoint;

		// taking the x axis as 0, how much we can rotate above and below it
		float rotationAnlgeAbove;
		float rotationAnlgeBelow;

		void Update();

		// pass in a relative position i.e. 0 - 1, and gives the position based on flip and rotaion
		VectorF GetPosition(VectorF relative_posision) const;
	};

	struct CoinStack
	{
		COMPONENT_TYPE(CoinStack)

		enum ColourType
		{
			White,
			Blue,
			Black,
			Red,
			Green,

			Count
		};

		ColourType colourType;
		SColour colour;

		int remaining;
		int capacity;
	};

	struct Inventory
	{
		COMPONENT_TYPE(Inventory)

		// amount of coins the player owns
		int coins[CoinStack::Count] { 0 };

		void Update();
	};

	// ----------------------------------------------------------------------
	// helpers
	static u64 archetypeBit(ECS::Component::Type type)
	{
		return (u64)1 << type;
	}

	Entity CreateEntity(const char* id, bool config_postfix = false);
	//Entity CreateEntity(const char* id, const char* config);
	Entity CreateEntity(const EntityMetaData& emd);

	const char* GetName(Entity entity);
	const Config* GetConfig(Entity entity);
	//const Config* GetConfigFromID(const char* id);

	Entity GetParent(Entity child);
	Entity GetFirstChild(Entity parent);
	VectorF GetPosition(Entity entity);
	RectF GetRect(Entity entity);
	bool GetRotationParams(Entity entity, VectorF& out_aboutPoint, float& out_rotation);

	// facing direction
	SDL_RendererFlip GetFacingDirection(Entity entity);
	VectorI GetFacingDirectionVector(Entity entity);

	void SetFacingDirection(Entity entity, SDL_RendererFlip direction);
	void FlipFacingDirection(Entity entity);
	SDL_RendererFlip GetDesiredFacingDirection(Entity entity, Entity target);

	VectorI FacingDirectionToVector(SDL_RendererFlip facing);
}