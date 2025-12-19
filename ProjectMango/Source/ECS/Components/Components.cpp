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
			//size = emd.size;
			size = config->data.GetVectorF("size_x", "size_y");

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

	void Transform::InitCollider(Collider& collider)
	{
		if(!size.isPositive())
			DebugPrint(Warning, "Transform size has to be set before init'ing collider");

		collider.InitFromTransform(*this);
	}
	
	void Transform::Init(const EntityMetaData* emd, Collider& collider)
	{
		Init(emd);
		InitCollider(collider);
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
		Transform& parent_transform = GetComponentRef(Transform, entity_data.parent);
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
	}

	void Transform::SetObjectCenter(VectorF pos)
	{
		VectorF object_size = size;

		if(Collider* collider = GetComponent(Collider, entity))
		{
			if(!collider->initialised)
				DebugPrint(Warning, "Collider has not been init'd, has no size");

			object_size = collider->rect.Size();
		}

		VectorF center_position = pos - (object_size * center);
		SetWorldPosition(center_position);
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
		flipPoint(VectorF(0.5f, 0.5f)),
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
			ID = id;
			SetTexture(id);
			canFlip = config->data.GetBool("can_flip", true);
		}
	}


	void Sprite::SetTexture(const char* label)
	{
		texture = TextureManager::Get()->getTexture(label, FileManager::Folder::Images);
		debugID = label;
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
	
	Entity Target::GetValidTarget(Entity entity)
	{
		if(Target* target_component = GetComponent(Target, entity))
		{
			if (ecs->IsAlive(target_component->target))
			{
				return target_component->target;
			}
		}

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
	

	// Spawner
	// ------------------------------------------------------------------
	Spawner::Spawner() : spawnedEntity(EntityInvalid), spawnRequest(EntityInvalid) { }

	//bool Spawner::Spawn( Entity entity_to_spawn )
	//{
	//	// begin the animation
	//	ECS::Animator& animator = GetComponentRef(Animator, entity);
	//	animator.StartAnimation(Action::Active);

	//	// hide it 
	//	entityToSpawn = entity_to_spawn;
	//	if(Sprite* sprite = GetComponent(Sprite, entityToSpawn))
	//	{
	//		sprite->colourMod.a = 0;
	//	}

	//	Physics& physics = AddComponent(Physics, entityToSpawn);
	//	physics.Init();

	//	//EntityState& character_state = GetComponentRef(EntityState, entityToSpawn);
	//	//character_state.character->SpawnIn(entityToSpawn);

	//	// position it on the spawner
	//	VectorF spawner_center = GetPosition(entity);
	//			 
	//	ECS::Transform& transform = GetComponentRef(Transform, entityToSpawn);
	//	VectorF translation = spawner_center - transform.GetObjectCenter();
	//	transform.SetWorldPosition(transform.worldPosition + translation);;

	//	// ray cast this rect onto the floow
	//	RectF rect = GetRect(entityToSpawn);

	//	float shift_y = 0.0f;
	//	float distance = 0.0f;
	//	if( RaycastToFloor(rect, distance) )
	//		shift_y = distance;

	//	transform.SetWorldPosition( transform.worldPosition + VectorF(0.0f, shift_y));

	//	return true;
	//}

	//void Spawner::Update()
	//{
	//	Animator& animator = GetComponentRef(Animator, entity);

	//	const Animation& animation = animator.GetActiveAnimation();
	//	if(animation.action == Action::Active)
	//	{
	//		// fade it in
	//		float alpha = (float)animator.frameIndex / (float)animation.frameCount;
	//		if(Sprite* sprite = GetComponent(Sprite, entityToSpawn))
	//		{
	//			sprite->colourMod.a = (Uint8)(c_alphaMax * alpha);
	//		}

	//		if(animator.loopCount > 0)
	//		{			
	//			if(Sprite* sprite = GetComponent(Sprite, entityToSpawn))
	//			{
	//				sprite->colourMod.a = c_alphaMax;
	//			}

	//			animator.StartAnimation(Action::Idle);
	//		}
	//	}
	//}
	

	// Door
	// ------------------------------------------------------------------
	Door::Door() : triggerRange(0)
	{
		colliders[0] = EntityInvalid;
		colliders[1] = EntityInvalid;
	}

	void Door::Init()
	{
		//Animator& animator = GetComponentRef(Animator, entity);
		//animator.StartAnimation(ActionState::Close);
	}

	void Door::GenerateColliders(float width)
	{
		//Entity top = CreateEntity("top door collider");
		//Entity bot = CreateEntity("bot door collider");
		//AddComponent(Transform, top);
		//AddComponent(Transform, bot);		
		//AddComponent(Collider, top);
		//AddComponent(Collider, bot);
		//colliders[0] = top;
		//colliders[1] = bot;

		//ECS::EntityData::SetParent(top, entity);
		//ECS::EntityData::SetParent(bot, entity);

		//const Transform& door_transform = GetComponentRef(Transform, entity);
		//const VectorF size(door_transform.size.x * width, door_transform.size.y * 0.5f);
		//const float x_pos = door_transform.worldPosition.x + door_transform.size.x * 0.5f - size.x * 0.5f;
		//const VectorF pos(x_pos, door_transform.worldPosition.y);
		//
		//Transform& top_transform = GetComponentRef(Transform, top);
		//Transform& bot_transform = GetComponentRef(Transform, bot);
		//Collider& top_collider = GetComponentRef(Collider, top);
		//Collider& bot_collider = GetComponentRef(Collider, bot);

		//VectorF door_part_size = VectorF(door_transform.size.x, door_transform.size.y * 0.5f);

		//top_transform.size = door_part_size;
		//top_transform.SetLocalPosition(VectorF(0,0));
		//top_transform.InitCollider(top_collider);
		//
		//VectorF relative_size = VectorF(width, 1.0f);
		//VectorF top_relative_pos = VectorF(0.5f - width * 0.5f, 0.0f);
		//top_collider.SetRelativeRect(top_relative_pos, relative_size);

		//bot_transform.size = door_part_size;
		//bot_transform.SetLocalPosition(VectorF(0, door_transform.size.y * 0.5f));
		//bot_transform.InitCollider(bot_collider);

		//VectorF bot_relative_pos = VectorF(0.5f - width * 0.5f, 0.0f);
		//bot_collider.SetRelativeRect(bot_relative_pos, relative_size);

		////top_collider.SetFlag(ECS::Collider::PlayerOnly);
		//top_collider.SetFlag(ECS::Collider::IsTerrain);
		////bot_collider.SetFlag(ECS::Collider::PlayerOnly);
		//bot_collider.SetFlag(ECS::Collider::IsTerrain);
	}

	void Door::Update()
	{
		//Animator& animator = GetComponentRef(Animator, entity);
		//const Animation& animation = animator.GetActiveAnimation();
	
		//Entity player_entity = Target::GetPlayer();
		//const VectorF player_pos = GetPosition(Target::GetPlayer());
		//if(!ecs->IsAlive(player_entity))
		//	return;

		//const RectF door_rect = GetRect(entity);
		//VectorF door_pos = door_rect.Center();

		//const bool withing_range_x = std::abs(player_pos.x - door_pos.x) < triggerRange;
		//const bool withing_range_z = std::abs(player_pos.y - door_pos.y) < door_rect.Height();
		//if(withing_range_x && withing_range_z)
		//{
		//	if(animation.action != ActionState::Open)
		//	{
		//		const int frame_index = animator.frameIndex;
		//		animator.StartAnimation(ActionState::Open);
		//		animator.frameIndex = animation.frameCount - frame_index;
		//	}
		//}
		//else
		//{
		//	if(animation.action != ActionState::Close)
		//	{
		//		const int frame_index = animator.frameIndex;
		//		animator.StartAnimation(ActionState::Close);
		//		animator.frameIndex = animation.frameCount - frame_index;
		//	}
		//}

		//float animation_progress = (float)animator.frameIndex / (float)(animation.frameCount - 1);
		//if(animation.action == ActionState::Close)
		//{
		//	animation_progress = 1 - animation_progress;
		//	if(animator.loopCount > 0)
		//		animation_progress = 0.0f;
		//}

		//const Transform& transform = GetComponentRef(Transform, entity);
		//float travel_distance = transform.size.y * 0.5f;

		//Transform& top_transform = GetComponentRef(Transform, colliders[0]);
		//VectorF top_local_start_position = VectorF(top_transform.localPosition.x, 0.0f);
		//VectorF top_local_current_position = top_local_start_position - VectorF(0, travel_distance * animation_progress);
		//top_transform.SetLocalPosition(top_local_current_position);
		//
		//Transform& bot_transform = GetComponentRef(Transform, colliders[1]);
		//VectorF bot_local_start_position = VectorF(bot_transform.localPosition.x, transform.size.y * 0.5f);
		//VectorF bot_local_current_position = bot_local_start_position + VectorF(0, travel_distance * animation_progress);
		//bot_transform.SetLocalPosition(bot_local_current_position);
	}


	// DeathScentence
	// ------------------------------------------------------------------
	DeathScentence::DeathScentence() : 
		deathTimer(-FLT_MAX), 
		deathZone(InvalidRectF), 
		deathLoops(-1),
		startAnimatiorOnDeath(EntityInvalid)
	{ }

	void DeathScentence::Update(float dt)
	{
		if(deathTimer != -FLT_MAX)
		{
			deathTimer -= dt;
		}

		// timer trigger
		if(deathTimer != -FLT_MAX)
		{
			if(deathTimer < 0)
			{
				OnDeath();
				return;
			}

		}

		// area trigger
		if( deathZone.isValid() )
		{
			if(Contains(deathZone, GetPosition(entity)))
			{
				OnDeath();
				return;
			}
		}

		// animator trigger
		if( deathLoops != -1)
		{
			if(const Animator* animator = GetComponent(Animator, entity))
			{
				if(animator->GetActiveAnimation().action == Action::Death)
				{
					if(animator->loopCount >= deathLoops)
					{
						OnDeath();
						return;
					}
				}
			}
		}
	}
		
	void DeathScentence::OnDeath()
	{
		if( startAnimatiorOnDeath != EntityInvalid )
		{
			Transform& transform = GetComponentRef(Transform, startAnimatiorOnDeath);
			transform.SetObjectCenter(GetPosition(entity));
						
			Animator& animator = GetComponentRef(Animator, startAnimatiorOnDeath);
			animator.state = TimeState::Running;
		}
		
		//canEnterDeathState = true;
		ecs->entities.KillEntity(entity);
	}
}