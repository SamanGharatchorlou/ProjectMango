#pragma once

#include "Core/TypeDefs.h"

#define ENTITY_LOGGING 1

namespace ECS
{
	constexpr u32 MaxEntityCount = 5000;

	using Entity = u32;		// simple id for each entity
	using Archetype = u64;	// bitwise encoding for every component type (max 64), a collection of types (key)
	using Signature = u64;	// a collection of type for a system, called its signature (lock)
	
	constexpr Entity EntityInvalid = -1;
	constexpr Archetype ArchetypeInvalid = -1;

	struct Component
	{
		enum Type : u64
		{
			EntityData,
			Transform,
			Sprite,
			CharacterState,
			PlayerController,
			Physics,
			Animator,
			Collider,
			AIController,
			Pathing,
			Damage,
			Health,
			Level,
			Spawner,

			Count
		};
	};

	static constexpr const char* ComponentNames[Component::Count]
	{
		"EntityData",
		"Transform", 
		"Sprite",
		"CharacterState", 
		"PlayerController", 
		"Physics", 
		"Animator",
		"Collider",
		"AIController",
		"Pathing",
		"Damage",
		"Health",
		"Level",
		"Spawner"
	};

#define COMPONENT_TYPE(comp) static Component::Type type() { return Component::comp; } \
							 Entity entity = EntityInvalid; \
	
	// lock - system signature
	// key - entity archetype
	static bool LockAndKey(Signature lock, Archetype key) 
	{
		return (lock & key) == lock;
	}

#define ArcheBit(compType) archetypeBit(compType::type())
}