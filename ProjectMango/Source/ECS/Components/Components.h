#pragma once

#include "Core/stack.h"
#include "Entities/States/CharacterAction.h"
#include "IdentityCommon.h"

class Texture;
struct ObjectConfig;

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
		BasicString config;

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
		// but it should be the visual one
		VectorF center;

		bool ignoreOutOfBounds;
		
		void Init(const ObjectConfig* config, VectorF pos, Collider& collider);
		void Init(const ObjectConfig* config, VectorF pos);
		void InitCollider(Collider& collider);

		void SetLocalPosition(VectorF pos);
		void SetWorldPosition(VectorF pos);
		void SetWorldRect(const VectorF& pos, const VectorF& size);

		void SetWorldPositionCenter(VectorF pos);

		VectorF GetObjectCenter() const;
		RectF GetRect() const;

		static VectorF GetObjectCenter(ECS::Entity entity);
	};

	struct Sprite
	{
		COMPONENT_TYPE(Sprite)

		RectF subRect;
		Texture* texture;
		
		VectorF flipPoint;
		SDL_RendererFlip flip;
		bool canFlip;
		
		// in degress (because of the render function input)
		float rotation; 
		u32 renderLayer;
		
		void Init(const ObjectConfig* config);
		bool IsFlipped() const { return flip == SDL_FLIP_HORIZONTAL; }
		void SetTexture(const char* label);
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
		
		void Init(const ObjectConfig* config);
		VectorI GetFacingDirection() const;
		void FlipFacingDirection();
	};

	struct PlayerController // more like a tag "I am a player"
	{
		COMPONENT_TYPE(PlayerController)
	};

	struct Pathing
	{
		COMPONENT_TYPE(Pathing)

		Entity target;

		VectorI currentStart;
		VectorI currentTarget;

		std::vector<VectorI> path;
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

		bool CanApplyTo(Entity entity) const;
		void ApplyTo(Entity entity);
	};

	struct Health
	{
		COMPONENT_TYPE(Health)

		float maxHealth;
		float currentHealth;

		bool invulnerable;
		
		void Init(const ObjectConfig* config);
		void ApplyDamage(const Damage& damage);
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

	// ----------------------------------------------------------------------
	// helpers
	static u64 archetypeBit(ECS::Component::Type type)
	{
		return (u64)1 << type;
	}

	Entity CreateEntity(const char* id, bool config_postfix = false);
	Entity CreateEntity(const char* id, const char* config);

	const char* GetName(Entity entity);
	const ObjectConfig* GetObjectConfig(Entity entity);
	const ObjectConfig* GetObjectConfigFromID(const char* id);

	Entity GetParent(Entity child);
	VectorF GetPosition(Entity entity);
	RectF GetRect(Entity entity);
	bool GetRotationParams(Entity entity, VectorF& out_aboutPoint, float& out_rotation);

}