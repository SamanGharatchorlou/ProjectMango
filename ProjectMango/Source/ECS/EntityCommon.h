#pragma once

#include "Core/TypeDefs.h"
#include "Game/Initialiser.h"

namespace ECS
{
	constexpr u32 MaxEntityCount = 1000;

	// WARNING - FIX THIS
	constexpr u32 MaxComponentIdCount = 100;

	using Entity = u32;		// simple id for each entity
	using Archetype = u64;	// bitwise encoding for every component type (max 64), a collection of types (key)
	using Signature = u64;	// a collection of type for a system, called its signature (lock)

	constexpr Entity EntityInvalid = -1;
	constexpr Archetype ArchetypeInvalid = -1;

	using ComponentID = u32;

	inline u32 ComponentCount = 0;

	template<typename T>
	ComponentID GetComponentID()
	{
		static ComponentID id = ComponentCount++;
		return id;
	}

	static constexpr u32 c_allEntities = 128;
	static constexpr u32 c_veryCommon = 64;
	static constexpr u32 c_common = 32;
	static constexpr u32 c_uncommon = 16;
	static constexpr u32 c_rare = 4;

#define COMPONENT_LIST(_entry) \
	_entry(EntityData,        c_common) \
	_entry(Transform,         c_allEntities) \
	_entry(Sprite,            c_allEntities) \
	_entry(SpriteSheet,       c_uncommon) \
	_entry(SpriteCycle,       c_uncommon) \
	_entry(LayeredSprite,     c_uncommon) \
	_entry(Animator,          c_veryCommon) \
	_entry(Jiggler,           c_rare) \
	_entry(Audio,             c_veryCommon) \
	_entry(EntityState,       c_common) \
	_entry(PlayerController,  c_rare) \
	_entry(Physics,           c_veryCommon) \
	_entry(Collider,          c_allEntities) \
	_entry(AIController,      c_common) \
	_entry(AIStrategy,		  c_rare) \
	_entry(UIIntentIcon,	  c_rare) \
	_entry(Pathing,           c_common) \
	_entry(Damage,            c_common) \
	_entry(Health,            c_veryCommon) \
	_entry(Biome,             c_rare) \
	_entry(Spawner,           c_rare) \
	_entry(SpawnRequest,      c_uncommon) \
	_entry(UICursor,          1) \
	_entry(UIButton,          c_uncommon) \
	_entry(UIText,            c_uncommon) \
	_entry(DeathScentence,    c_common) \
	_entry(Inventory,         c_rare) \
	_entry(CoinStack,         Colour::Count) \
	_entry(Card,              c_uncommon) \
	_entry(TurnState,         c_rare) \
	_entry(ActionRequest,     c_rare) \
	_entry(Callback,          c_uncommon) \
	_entry(Colour,            c_uncommon) \
	_entry(AIIntent,          c_common) \
	_entry(Faction,           c_common) \
	_entry(BehaviourMap,      c_common) \
	_entry(BehaviourState,    c_common) \
	_entry(StatusEffects,	  c_rare)


#define STRING_ENTRY(name, _) #name,

	static const char* ComponentNames[] =
	{
		COMPONENT_LIST(STRING_ENTRY)
	};

#define COMPONENT_TYPE(comp) Entity entity = EntityInvalid; \
	static StringBuffer32 TypeName() { const char* full_name = typeid(comp).name();  \
										const char* short_name = strrchr(full_name, ':'); \
										return short_name ? short_name + 1 : full_name; } \
	static ComponentID TypeId() { return GetComponentID<comp>(); }
							 
#define DEFINE_COMPONENT(component, size) \
	struct component##initialiser : public ComponentInitialiser { \
		u32 GetType() override { return GetComponentID<component>(); } \
		void OnInit() override { RegisterComponent(component, size); } \
		void Remove(ECS::Entity entity) { RemoveComponent(component, entity);  } }; \
	static component##initialiser s_##component##initialiser;

	// lock - system signature
	// key - entity archetype
	static bool LockAndKey(Signature lock, Archetype key) 
	{
		return (lock & key) == lock;
	}

#define ArcheBit(component) archetypeBit(GetComponentID<component>())

}