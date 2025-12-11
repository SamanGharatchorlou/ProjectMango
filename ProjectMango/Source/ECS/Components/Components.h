#pragma once

#include "Core/stack.h"
#include "Entities/States/CharacterAction.h"
#include "ComponentHelpers.h"

class STexture;
struct Config;

// when adding a component, define it in EntityCommon.h
// then setup how its updated in ComponentsSetup

namespace ECS
{
	struct Collider;

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

	struct Sprite
	{
		COMPONENT_TYPE(Sprite)

		BasicString debug_id;

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

	struct Coin
	{
		enum Type
		{
			White,
			Blue,
			Black,
			Red,
			Green,
			Count
		};

		inline static const std::unordered_map<StringBuffer32, Type> s_stringToType { 
			{ "White",	White }, 
			{ "Blue",	Blue }, 
			{ "Black",	Black },
			{ "Red",	Red },
			{ "Green",	Green } 
		};

		inline static const std::unordered_map<Type, StringBuffer32> s_typeToString { 
			{ White,	"White" }, 
			{ Blue,		"Blue" }, 
			{ Black,	"Black" },
			{ Red,		"Red" },
			{ Green,	"Green" } 
		};
				
		inline static const std::unordered_map<Type, SColour> s_typeToColour { 
			{ White,	SColour::White }, 
			{ Blue,		SColour::Blue }, 
			{ Black,	SColour::Black },
			{ Red,		SColour::Red },
			{ Green,	SColour::Green } 
		};
	};

	struct CoinStack
	{
		COMPONENT_TYPE(CoinStack)

		Coin::Type coinType;
		SColour colour;

		int remaining;
		int capacity;

		static CoinStack* GetCoinStack(Coin::Type type);
	};

	struct Card
	{
		static constexpr int c_tiers = 3;

		COMPONENT_TYPE(Card)

		Coin::Type colour;

		// what to pay to aquire the card
		int cost[Coin::Count] { 0 };

		// how many coins it provides once owned
		int power[Coin::Count] { 0 };

		// points... for something, not sure yet
		int points;

		// tier 1,2,3
		int tier;
	};

	struct Inventory
	{
		COMPONENT_TYPE(Inventory)

		// amount of coins owned owns
		int coins[Coin::Count] { 0 };

		// cards we own
		std::vector<Card> cards;

		void GetCardPower(int array[], int size) const;
		void GetBuyingPower(int array[], int size) const; 
	};

	struct TurnState
	{
		COMPONENT_TYPE(TurnState)

		int turnIndex;
		int initiative;

		int collectedCoins[Coin::Count] { 0 };
		Entity collectedCard;

		void ResetState();
	};

	struct ActionRequest
	{
		COMPONENT_TYPE(ActionRequest)

		enum Type
		{
			None,
			CollectCoin,
			AquireCard,
			EndTurn,
			UndoTurn
		};

		Type request;

		// coin stack to collect from, card to aquire
		Entity target;
	};
}