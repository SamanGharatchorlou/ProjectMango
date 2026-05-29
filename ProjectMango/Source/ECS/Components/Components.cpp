#include "pch.h"
#include "Components.h"

#include "Core/Helpers.h"
#include "Core/Maths.h"
#include "ECS/Components/GraphicComponents.h"
#include "ECS/Components/SpacialComponents.h"
#include "ECS/EntityCommon.h"
#include "ECS/EntityCoordinator.h"
#include "System/Files/ConfigManager.h"
#include "Audio/AudioManager.h"

#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"


namespace ECS
{
	// EntityData
	// ------------------------------------------------------------------
	EntityData::EntityData() : parent(EntityInvalid)
	{
		iid = Maths::GenerateIID();
	}

	void EntityData::Init(const EntityMetaData& emd)
	{
		if (emd.data.Contains(kRequirement))
			id = emd.GetID();

		if (emd.data.Contains("iid"))
			iid = emd.data.GetU64("iid");
	}
	void EntityData::Serialise(EntityMetaData& out_emd) const
	{
		out_emd.data.AddString(kRequirement, id.c_str());
		out_emd.data.AddU64("iid", iid);

		for (u32 i = 0; i < children.size(); i++)
		{
			if (EntityData* child_ed = GetComponent(EntityData, children[i]))
			{
				out_emd.data.intArrays["children_iids"].push_back(child_ed->iid);
			}
		}
	}

	void EntityData::SetParent(Entity child, Entity parent)
	{
		// todo: now that I default add this to every entity i can remove this first check right?
		// set new entity parent
		EntityData* entity_data = GetComponent(EntityData, child);
		if(!entity_data)
		{
			AddComponent(EntityData, child);
			entity_data = GetComponent(EntityData, child);
		}

		// remove ourself from the old parent, if there was one
		if( entity_data->parent != EntityInvalid )
		{
			EntityData& old_parent_entity_data = GetComponentRef(EntityData, entity_data->parent);
			EraseSwap(old_parent_entity_data.children, child);
		}

		entity_data->parent = parent;
		
		// add the child to the parent
		EntityData* parent_entity_data = GetComponent(EntityData, parent);
		if(!parent_entity_data)
		{
			AddComponent(EntityData, parent);
			parent_entity_data = GetComponent(EntityData, parent);
		}

		PushBackUnique(parent_entity_data->children, child);

		// update the local position
		if(Transform* transform = GetComponent(Transform, child))
			transform->SetWorldPosition(transform->worldPosition);
	}


	
	
	// Audio
	// ------------------------------------------------------------------
	void Audio::Play(const char* sound_effect)
	{
		const Group& group = soundEffects.at(sound_effect);

		int index = Maths::randomNumberBetween(0, (int)group.sounds.size());
		if(index >= 0)
		{
			AudioManager* am = AudioManager::Get();
			am->PlaySoundEffect( group.sounds[index].c_str(), group.time );
		}
	}

	void Audio::PopulateGroup(const char* group_id, int time, const char* id)
	{
		if(soundEffects.contains(id))
		{
			DebugPrint(Warning, "Populating group with an already existing id: '%s'", id);
		}
		
		AudioManager* am = AudioManager::Get();
		Group& group = soundEffects[id];
		group.time = time;

		char buffer[64] { 0 };

		int counter = 1;
		bool has_entry = true;
		while(has_entry)
		{
			snprintf(buffer, 64, "%s-%d", group_id, counter++);
			if(am->GetSoundEffectNoError(buffer))
			{
				group.sounds.push_back(buffer);
			}
			else
			{
				has_entry = false;
			}
		}
	}

	
	// Target
	// ------------------------------------------------------------------
	void Faction::Init(const EntityMetaData& emd)
	{
		team = Faction::GetTeam(emd.data.GetString(kRequirement));
	}
	
	Entity Faction::GetTarget() const
	{
		Entity target_entity = EntityInvalid;

		if(team == Team::Player)
		{
			target_entity = GetEnemy();
		}
		else if(team == Team::Enemy)
		{
			target_entity = GetPlayer();
		}
					
		if (ecs->IsAlive(target_entity))
		{
			if(const Health* health = GetComponent(Health, target_entity))
			{
				if(health->currentHealth <= 0)
					return EntityInvalid;
			}
			return target_entity;
		}

		return EntityInvalid;
	}

	
	Entity Faction::GetTarget(Entity entity)
	{
		if(Faction* faction = GetComponent(Faction, entity) )
		{
			return faction->GetTarget();
		}

		return EntityInvalid;
	}

	Entity Faction::GetPlayer()
	{
		ComponentArray<PlayerController>& players =  GetAllComponents(PlayerController);
		for( auto iter = players.entityToComponent.begin(); iter != players.entityToComponent.end(); iter++ )
		{
			return iter->first;
		}

		return EntityInvalid;
	}
	Entity Faction::GetEnemy()
	{
		State& state = GameData::Get().systemStateManager->mStates.getActiveState();
		if(GameState* game_state = GameState::GetActive())
			return game_state->enemy;

		return EntityInvalid;
	}

	void Faction::SetAsHostileFaction(Entity entity, Faction& hostile_faction)
	{
		if(const Faction* entity_faction = GetComponent(Faction, entity) )
		{
			if(entity_faction->team == Team::Player)
				hostile_faction.team = Team::Enemy;
			else if(entity_faction->team == Team::Player)
				hostile_faction.team = Team::Enemy;
		}
	}

	void Faction::SetAsAlliedFaction(Entity entity, Faction& hostile_faction)
	{
		if(const Faction* entity_faction = GetComponent(Faction, entity) )
		{
			if(entity_faction->team != Team::None)
				hostile_faction.team = entity_faction->team;
		}
	}
	
	Faction::Team Faction::GetTeam(Entity entity)
	{
		if(Faction* faction = GetComponent(Faction, entity) )
		{
			return faction->team;
		}

		return Team::None;
	}
	
	Faction::Team Faction::GetTeam(const char* faction)
	{
		if(StringCompare(faction, "Player"))
			return Faction::Player;
		else if(StringCompare(faction, "Enemy"))
			return Faction::Enemy;

		return Faction::None;
	}

	void Faction::DebugGetFactionName(Entity entity, BasicString& name)
	{
		Faction::Team team = GetTeam(entity);
		if(team == Faction::Player)
			name = "Player";
		if(team == Faction::Enemy)
			name = "Enemy";

		name = "None";
	}

	
	// Damage
	// ------------------------------------------------------------------
	Damage::Damage() : value(0), force(0) 
	{ }

	
	void Damage::Init()
	{
		if(const Config* config = GetConfigFromEntity(entity))
			value = config->data.GetFloat("damage");
	}

	bool Damage::CanApplyTo(Entity _entity) const
	{
		for( u32 i = 0; i < appliedTo.size(); i++ )
		{
			// dont re-apply damage to this source
			if(appliedTo[i] == _entity)
				return false;
		}

		return true;
	}

	void Damage::ApplyTo(Entity _entity)
	{
		PushBackUnique(appliedTo, _entity);

		if(Health* health = GetComponent(Health, _entity))
			health->ApplyDamage(value);

		if(Physics* physics = GetComponent(Physics, _entity))
		{
			float speed = physics->speed.x;

			const Transform& transform = GetComponentRef(Transform, _entity);

			const float force_direction = source.x < transform.GetObjectCenter().x ? force : -force;
			const float impulse = force_direction / physics->mass;
			physics->speed += VectorF(impulse, 0.0f);
		}
	}


	// Health
	// ------------------------------------------------------------------
	Health::Health() : maxHealth(0), currentHealth(0), invulnerable(false) { }

	void Health::Init()
	{
		if(const Config* config = GetConfigFromEntity(entity))
		{
			maxHealth = config->data.GetFloat("max_health");
		}
		currentHealth = maxHealth;
	}

	void Health::Init(const EntityMetaData& emd)
	{
		maxHealth = emd.data.GetInt(kRequirement);
		currentHealth = maxHealth;
	}

	bool Health::ApplyDamage(float damage)
	{
		if(invulnerable)
			return false;

		if(currentHealth > 0)
		{
			currentHealth -= damage;
			currentHealth = std::clamp(currentHealth, 0.0f, maxHealth);
			return true;
		}

		return false;
	}

	// DeathScentence
	// ------------------------------------------------------------------
	bool DeathScentence::CanDie()
	{
		// timer trigger
		if(deathTimer != -FLT_MAX)
		{
			if(deathTimer > 0)
			{
				return false;
			}
		}

		return true;
	}
	
	// Callback
	// ------------------------------------------------------------------
	void Callback::Init(const EntityMetaData& emd)
	{
		callback = emd.data.GetString(kRequirement);
	}

	void Callback::Serialise(EntityMetaData& out_emd) const
	{
		out_emd.data.strings[kRequirement] = callback;
	}
}