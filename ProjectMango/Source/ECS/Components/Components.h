#pragma once

#include "ComponentHelpers.h"

// when adding a component, define it in EntityCommon.h
// then setup how its updated in ComponentsSetup

// more generic components go here
namespace ECS
{
	struct EntityData
	{
		COMPONENT_TYPE(EntityData)
		
		EntityData();

		BasicString id;

		ECS::Entity parent;
		std::vector<Entity> children;

		static void SetParent(Entity child, Entity parent);
	};

	struct Audio
	{
		COMPONENT_TYPE(Audio)

		static constexpr const char* c_noId = "";

		struct Group
		{
			std::vector<BasicString> sounds;
			int time = 0;
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

		bool justChanged = false;

		// forces the state into the backlog
		bool pushStateToBacklog = false;
	};

	struct Faction
	{
		COMPONENT_TYPE(Faction)
			
		static constexpr const char* kRequirement = "faction";
		void Init(const EntityMetaData& emd);

		enum Team
		{
			None,
			Player,
			Enemy,
			Count
		};

		Team team; 

		Entity GetTarget() const;
		static Entity GetTarget(Entity entity);

		static Entity GetPlayer();
		static Entity GetEnemy();
		static Team GetTeam(Entity entity);
		static Team GetTeam(const char* faction);

		static void SetAsHostileFaction(Entity entity, Faction& hostile_faction);
		static void SetAsAlliedFaction(Entity entity, Faction& hostile_faction);

		static void DebugGetFactionName(Entity entity, BasicString& name);
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

		Entity sourceEntity = EntityInvalid;
		int hitFrame = -1;

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

		static constexpr const char* kRequirement = "max_health";

		Health();

		float maxHealth;
		float currentHealth;

		bool invulnerable;
		
		void Init();
		void Init(const EntityMetaData& emd);
		bool ApplyDamage(float damage);
	};


	struct DeathScentence
	{
		COMPONENT_TYPE(DeathScentence)

		// kill at animator loop count
		int deathLoops = -1;
		Action::Enum action = Action::Death;

		// kill on timer
		float deathTimer = -FLT_MAX;

		float fadeOutTime = 0.0f;
		float fadeOutTimer = 0.0f;
		
		bool CanDie();
	};

	struct Callback
	{
		COMPONENT_TYPE(Callback)

		static constexpr const char* kRequirement = "callback";
		void Init(const EntityMetaData& emd);

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