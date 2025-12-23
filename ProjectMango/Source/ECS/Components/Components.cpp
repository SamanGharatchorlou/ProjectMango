#include "pch.h"
#include "Components.h"

#include "Core/Helpers.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Biome.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Physics.h"
#include "ECS/EntityCommon.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/EntSystems/TransformSystem.h"
#include "Graphics/TextureManager.h"
#include "System/Files/ConfigManager.h"
#include "Audio/AudioManager.h"
#include "Graphics/Raycast.h"

#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"
#include "Graphics/STexture.h"

namespace ECS
{
	// EntityData
	// ------------------------------------------------------------------
	EntityData::EntityData() : parent(EntityInvalid)
	{ }

	void EntityData::SetParent(Entity entity, Entity parent)
	{
		// todo: now that I default add this to every entity i can remove this first check right?
		// set new entity parent
		EntityData* entity_data = GetComponent(EntityData, entity);
		if(!entity_data)
		{
			AddComponent(EntityData, entity);
			entity_data = GetComponent(EntityData, entity);
		}

		// remove ourself from the old parent, if there was one
		if( entity_data->parent != EntityInvalid )
		{
			EntityData& old_parent_entity_data = GetComponentRef(EntityData, entity_data->parent);
			EraseSwap(old_parent_entity_data.children, entity);
		}

		entity_data->parent = parent;
		
		// add the child to the parent
		EntityData* parent_entity_data = GetComponent(EntityData, parent);
		if(!parent_entity_data)
		{
			AddComponent(EntityData, parent);
			parent_entity_data = GetComponent(EntityData, parent);
		}

		PushBackUnique(parent_entity_data->children, entity);
	}


	// Transform
	// ------------------------------------------------------------------
	Transform::Transform() : 
		ignoreOutOfBounds(false),
		center(0.5f, 0.5f)
	{ }

	void Transform::Init(const EntityMetaData* emd)
	{
		if(const Config* config = GetConfigFromEntity(entity))
		{
			size = config->data.GetVector("size");

			if(emd)
			{
				VectorF pos = emd->position - (size * emd->pivotPoint);

				SetWorldPosition(pos);

				if(config->data.GetBool("snap_to_floor"))
				{
					float distance = 0.0f;
					if( RaycastToFloor(entity, distance) )
					{
						// shift up 1 just so we're not inside the floor collider
						SetWorldPosition( pos + VectorF(0.0f, distance));
					}
				}
			}
		}
	}
	
	void Transform::Init(const EntityMetaData* emd, Collider& collider)
	{
		Init(emd);
		collider.Init();
	}

	void Transform::SetWorldRect(const VectorF& _pos, const VectorF& _size)
	{
		size = _size;
		SetWorldPosition(_pos);
	}

	void Transform::SetLocalPosition(VectorF pos)
	{
		ASSERT(!size.isZero(), "Make sure to set the size BEFORE you set the local position");

		localPosition = pos;
		
		// must have a parent by this point
		EntityData& entity_data = GetComponentRef(EntityData, entity);
		const Transform& parent_transform = GetComponentRef(Transform, entity_data.parent);
		SetWorldPosition(parent_transform.worldPosition + localPosition);

		TransformSystem::UpdateChildrenTransforms(entity_data.parent);
	}

	void Transform::SetWorldPosition(VectorF pos)
	{
		targetWorldPosition = pos;
		worldPosition = pos;
		
		// update collider positions
		if (Collider* collider = GetComponent(Collider, entity))
		{
			collider->UpdateFromTransform(*this);
		}

		
		EntityData& entity_data = GetComponentRef(EntityData, entity);
		if(entity_data.parent != EntityInvalid)
		{
			const Transform& parent_transform = GetComponentRef(Transform, entity_data.parent);
			localPosition = worldPosition - parent_transform.worldPosition;
		}
	}

	void Transform::SetObjectCenter(VectorF pos)
	{
		VectorF object_size = size;
		VectorF object_offset = object_size * center;

		if(Collider* collider = GetComponent(Collider, entity))
		{
			if(!collider->initialised)
				DebugPrint(Warning, "Collider has not been init'd, has no size");

			object_offset = collider->rect.Center() - worldPosition;
		}

		SetWorldPosition(pos - object_offset);
	}

	
	RectF Transform::GetObjectRect() const
	{
		if(const Collider* collider = GetComponent(Collider, entity))
		{
			return collider->rect;
		}
		else
		{
			return GetRect();
		}
	}

	
	VectorF Transform::GetHorizontalFlipPoint() const
	{
		if(const Collider* collider = GetComponent(Collider, entity))
		{
			RectF relative_rect = collider->GetRelativeRect();
			VectorF flip = relative_rect.TopLeft() + (relative_rect.Size() * 0.5f);
			return VectorF(flip.x, 0.5f) * size;
		}
		else
		{
			return VectorF(0.5f, 0.5f);
		}
	}

	VectorF Transform::GetObjectCenter() const
	{
		if(const Collider* collider = GetComponent(Collider, entity))
		{
			return collider->rect.Center();
		}
		else
		{
			return worldPosition + size * 0.5f;
		}

		return VectorF::zero();
	}

	VectorF Transform::GetObjectCenter(ECS::Entity entity)
	{
		const Transform& transform = GetComponentRef(Transform, entity);
		return transform.GetObjectCenter();
	}
	
	RectF Transform::GetRect() const
	{
		return RectF(worldPosition, size);
	}

	VectorF Transform::GetRelativePosition(VectorF relative) const
	{
		return worldPosition + (relative * size);
	}

	// Sprite
	// ------------------------------------------------------------------
	Sprite::Sprite() :
		texture(nullptr),
		//flipPoint(VectorF(0.5f, 0.5f)),
		flip(SDL_FLIP_NONE),
		canFlip(true),
		rotation(0),
		renderLayer(RenderLayer::None),
		disabled(false)
	{ }

	void Sprite::Init()
	{
		if(const Config* config = GetConfigFromEntity(entity))
		{
			const char* id = config->data.GetString("sprite");
			Id = id;
			SetTexture(id);
			canFlip = config->data.GetBool("can_flip", true);
		}
	}


	void Sprite::SetTexture(const char* label)
	{
		texture = TextureManager::Get()->getTexture(label, FileManager::Folder::Images);
		debugID = label;
	}

	
	void SpriteSheet::Init(const char* sprite_sheet, int _count)
	{
		Id = sprite_sheet;
		count = _count;
		if(texture = TextureManager::Get()->getTexture(sprite_sheet, FileManager::Folder::Images))
			frameSize = texture->originalDimentions / VectorF((float)_count, 1.0f);
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
	Entity Target::GetTarget() const
	{
		Entity target_entity = targetEntity;

		if(isEnemy)
		{
			target_entity = GetEnemy();
		}
		else if(isPlayer)
		{
			target_entity = GetPlayer();
		}
					
		if (ecs->IsAlive(target_entity))
			return target_entity;

		return EntityInvalid;
	}

	
	Entity Target::GetTarget(Entity entity)
	{
		if(Target* target = GetComponent(Target, entity) )
		{
			return target->GetTarget();
		}

		return EntityInvalid;
	}

	Entity Target::GetPlayer()
	{
		ComponentArray<PlayerController>& players =  GetAllComponents(PlayerController);
		for( auto iter = players.entityToComponent.begin(); iter != players.entityToComponent.end(); iter++ )
		{
			return iter->first;
		}

		return EntityInvalid;
	}
	Entity Target::GetEnemy()
	{
		State& state = GameData::Get().systemStateManager->mStates.getActiveState();
		if(GameState* game_state = dynamic_cast<GameState*>(&state))
			return game_state->enemy;

		return EntityInvalid;
	}

	// EntityState
	// ------------------------------------------------------------------


	
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

	void Health::ApplyDamage(float damage)
	{
		if(invulnerable)
			return;

		currentHealth -= damage;
		currentHealth = std::clamp(currentHealth, 0.0f, maxHealth);
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

	void DeathScentence::Update(float dt)
	{
		if(deathTimer != -FLT_MAX)
		{
			deathTimer -= dt;
		}

		if(!CanDie())
			return;

		// animator trigger
		if( deathLoops != -1)
		{
			if(const Animator* animator = GetComponent(Animator, entity))
			{
				if(animator->GetActiveAnimation().action == Action::Death)
				{
					if(animator->loopCount >= deathLoops)
					{
						ecs->entities.KillEntity(entity);
						return;
					}
				}
			}
		}
	}
}