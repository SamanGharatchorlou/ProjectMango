#pragma once

#include "Core/stack.h"
#include "Characters/States/CharacterAction.h"

class Texture;
struct ObjectConfig;

namespace ECS
{
	enum Direction { Up, Right, Down, Left, Count };

	static const VectorI s_directions[Direction::Count] 
	{ 
		VectorI(0,-1), VectorI( 1, 0), VectorI(0, 1), VectorI(-1, 0) 
	};

	struct EntityData
	{
		COMPONENT_TYPE(EntityData)

		ECS::Entity parent = EntityInvalid;
		std::vector<Entity> children;

		static void SetParent(Entity entity, Entity parent);
	};

	struct Transform
	{
		COMPONENT_TYPE(Transform)

		VectorF targetWorldPosition;
		VectorF worldPosition;
		VectorF localPosition;

		VectorF renderOffset;

		VectorF size;

		bool ignoreOutOfBounds = false;
		
		void Init(const SettingValues& values, VectorF pos);

		void SetLocalPosition(VectorF pos);
		void SetWorldPosition(VectorF pos);
		void SetWorldRect(const VectorF& pos, const VectorF& size);

		VectorF GetObjectCenter() const;
		RectF GetRect() const;
	};

	struct Sprite
	{
		COMPONENT_TYPE(Sprite)

		RectF subRect;
		Texture* texture = nullptr;
		
		VectorF flipPoint = VectorF(0.5f, 0.5f);
		SDL_RendererFlip flip;
		bool canFlip = true;

		u32 renderLayer = 0;
		
		bool IsFlipped() const { return flip == SDL_FLIP_HORIZONTAL; }
	};
	
	struct CharacterState
	{
		COMPONENT_TYPE(CharacterState)

		ActionStack<CharacterAction> actions;

		BasicString config;

		// overload common functions
		Character* character;

		VectorI movementInput;
		VectorI GetFacingDirection() const;

		template<class T>
		const T* GetConfig() const { return ConfigManager::Get()->GetConfig<T>(config.c_str()); }
	};

	struct Pathing
	{
		COMPONENT_TYPE(Pathing)

		Entity target = ECS::EntityInvalid;

		VectorI currentStart;
		VectorI currentTarget;

		std::vector<VectorI> path;
	};

	struct Damage
	{
		COMPONENT_TYPE(Damage)

		// the damage
		float value = 0;

		// may apply a force
		float force;
		VectorF source;

		// which entity we've already applied to, dont apply again
		std::vector<Entity> appliedTo;

		bool CanApplyTo(Entity entity) const;
		void ApplyTo(Entity entity);
	};

	struct Health
	{
		COMPONENT_TYPE(Health)

		float maxHealth = 0.0f;
		float currentHealth = 0.0f;

		bool invulnerable = false;
		
		void Init(const SettingValues& values);
		void ApplyDamage(const Damage& damage);
	};

	
	typedef Entity (*EntitySpawnFn)( const char* id, const char* config );

	struct Spawner
	{
		COMPONENT_TYPE(Spawner)

		EntitySpawnFn entitySpawnFn = nullptr;
		const char* spawnId = nullptr;
		const char* spawnConfig = nullptr;

		bool IsSpawning() { return entitySpawnFn != nullptr; }

		bool Spawn(const char* spawn_id, const char* spawn_config, EntitySpawnFn spawnFn);
		void Update();
	};

	struct Door
	{
		COMPONENT_TYPE(Door)

		float triggerRange = 0.0f;

		// top and bottom
		Entity colliders[2];

		void Init();
		void Update();

		void GenerateColliders(float width);
	};

	// ----------------------------------------------------------------------
	// helpers
	static u64 archetypeBit(ECS::Component::Type type)
	{
		return (u64)1 << type;
	}

	Entity GetParent(Entity child);
	VectorF GetPosition(Entity entity);
	RectF GetRect(Entity entity);

	const ObjectConfig* GetObjectConfig(Entity entity);
}