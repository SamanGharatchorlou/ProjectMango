#pragma once

#include "Core/TypeDefs.h"
#include "Game/Initialiser.h"

namespace ECS
{
	constexpr u32 MaxEntityCount = 1000;

	using Entity = u32;		// simple id for each entity
	using Archetype = u64;	// bitwise encoding for every component type (max 64), a collection of types (key)
	using Signature = u64;	// a collection of type for a system, called its signature (lock)
	
	constexpr Entity EntityInvalid = -1;
	constexpr Archetype ArchetypeInvalid = -1;

#define ENUM_ENTRY( e ) e,
#define STRING_ENTRY( t ) #t,

#define COMPONENTS( _entry ) \
			_entry( EntityData ) \
			_entry( Transform ) \
			_entry( Sprite ) \
			_entry( Audio ) \
			_entry( CharacterState ) \
			_entry( PlayerController ) \
			_entry( Physics ) \
			_entry( Animator ) \
			_entry( Collider ) \
			_entry( AIController ) \
			_entry( Pathing ) \
			_entry( Damage ) \
			_entry( Health ) \
			_entry( Biome ) \
			_entry( Spawner ) \
			_entry( Door ) \
			_entry( UICursor ) \
			_entry( UIButton ) \
			_entry( SpellBook ) \
			_entry( Spell ) \
			_entry( Pickup ) \
			_entry( DeathScentence ) \
			_entry( Firearm ) \
			_entry( Arm ) \


	struct Component
	{
		enum Type : u64
		{
			COMPONENTS( ENUM_ENTRY )
			Count
		};
	};
	
	static const char* ComponentNames[Component::Count]
	{
		COMPONENTS( STRING_ENTRY )
	};

#define COMPONENT_TYPE(comp) static Component::Type type() { return Component::comp; } \
							 Entity entity = EntityInvalid; \
							 comp();

#define DEFINE_COMPONENT( component, size ) \
	struct component##initialiser : public ComponentInitialiser { \
		u32 GetType() override { return Component::component; } \
		void OnInit() override { RegisterComponent(component, size); } \
		void Remove(ECS::Entity entity) { RemoveComponent(component, entity);  } }; \
	static component##initialiser s_##component##initialiser;

	// lock - system signature
	// key - entity archetype
	static bool LockAndKey(Signature lock, Archetype key) 
	{
		return (lock & key) == lock;
	}

#define ArcheBit(compType) archetypeBit(ECS::compType::type())
}