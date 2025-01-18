#include "pch.h"
#include "Components.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Physics.h"
#include "Core/Helpers.h"
#include "ECS/Components/Collider.h"
#include "ECS/EntSystems/TransformSystem.h"
#include "ECS/Components/Animator.h"
#include "Animations/CharacterStates.h"
#include "Entities/Player/PlayerCharacter.h"
#include "System/Files/ConfigManager.h"
#include "Graphics/TextureManager.h"

#include "ECS/EntityCommon.h"

namespace ECS
{
	// EntityData
	// ------------------------------------------------------------------
	EntityData::EntityData() : 
		parent(EntityInvalid) 
	{ }

	void EntityData::SetParent(Entity entity, Entity parent)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		// set new entity parent
		EntityData* entity_data = ecs->GetComponent(EntityData, entity);
		if(!entity_data)
		{
			ecs->AddComponent(EntityData, entity);
			entity_data = ecs->GetComponent(EntityData, entity);
		}

		// remove ourself from the old parent, if there was one
		if( entity_data->parent != EntityInvalid )
		{
			EntityData& old_parent_entity_data = ecs->GetComponentRef(EntityData, entity_data->parent);
			EraseSwap(old_parent_entity_data.children, entity);
		}

		entity_data->parent = parent;
		
		// add the child to the parent
		EntityData* parent_entity_data = ecs->GetComponent(EntityData, parent);
		if(!parent_entity_data)
		{
			ecs->AddComponent(EntityData, parent);
			parent_entity_data = ecs->GetComponent(EntityData, parent);
		}

		PushBackUnique(parent_entity_data->children, entity);
	}


	// Transform
	// ------------------------------------------------------------------
	Transform::Transform() : 
		ignoreOutOfBounds(false) 
	{ }

	void Transform::Init(const SettingValues& values, VectorF pos)
	{
		size = values.GetVectorF("size_x", "size_y");
		SetWorldPosition(pos);

		if(values.GetBool("snap_to_floor"))
		{
			float distance = 0.0f;
			if( RaycastToFloor(entity, distance) )
			{
				// shift up 1 just so we're not inside the floor collider
				SetWorldPosition( pos + VectorF(0.0f, distance));
			}
		}
	}

	void Transform::InitCollider(Collider& collider)
	{
		if(!size.isPositive())
			DebugPrint(Warning, "Transform size has to be set before init'ing collider");

		collider.InitFromTransform(*this);
	}
	
	void Transform::Init(const SettingValues& values, VectorF pos, Collider& collider)
	{
		Init(values,pos);
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
		
		EntityCoordinator* ecs = GameData::Get().ecs;

		// must have a parent by this point
		EntityData& entity_data = ecs->GetComponentRef(EntityData, entity);
		Transform& parent_transform = ecs->GetComponentRef(Transform, entity_data.parent);
		SetWorldPosition(parent_transform.worldPosition + localPosition);

		TransformSystem::UpdateChildrenTransforms(entity_data.parent);
	}

	void Transform::SetWorldPosition(VectorF pos)
	{
		targetWorldPosition = pos;
		worldPosition = pos;
		
		// update collider positions
		EntityCoordinator* ecs = GameData::Get().ecs;
		if (Collider* collider = ecs->GetComponent(Collider, entity))
		{
			collider->UpdateFromTransform(this);
		}
	}

	void Transform::SetWorldPositionCenter(VectorF pos)
	{
		VectorF object_size = size;

		EntityCoordinator* ecs = GameData::Get().ecs;
		if(Collider* collider = ecs->GetComponent(Collider, entity))
		{
			if(!collider->initialised)
				DebugPrint(Warning, "Collider has not been init'd, has no size");

			object_size = collider->rect.Size();
		}

		VectorF center_position = pos - (object_size * center);
		SetWorldPosition(center_position);
	}

	VectorF Transform::GetObjectCenter() const
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		if(const Collider* collider = ecs->GetComponent(Collider, entity))
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
		EntityCoordinator* ecs = GameData::Get().ecs;
		const Transform& transform = ecs->GetComponentRef(Transform, entity);
		return transform.GetObjectCenter();
	}
	
	RectF Transform::GetRect() const
	{
		return RectF(worldPosition, size);
	}


	// Sprite
	// ------------------------------------------------------------------
	Sprite::Sprite() :
		texture(nullptr),
		flipPoint(VectorF(0.5f, 0.5f)),
		flip(SDL_FLIP_NONE),
		canFlip(true),
		rotation(0),
		renderLayer(0)
	{ }

	void Sprite::SetTexture(const char* label)
	{
		texture = TextureManager::Get()->getTexture(label, FileManager::Folder::Images);
	}


	// CharacterState
	// ------------------------------------------------------------------
	CharacterState::CharacterState() :
		character(nullptr),
		isRanged(true),
		isMelee(false),
		canEnterHover(false)
	{ }

	void CharacterState::Init(const SettingValues& values)
	{
		isRanged = values.GetBool("ranged", true);
		isMelee = values.GetBool("melee", false);
	}

	VectorI CharacterState::GetFacingDirection() const
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		Sprite& sprite = ecs->GetComponentRef(Sprite, entity);

		int direction = sprite.IsFlipped() ? -1 : 1;

		return VectorI(direction, 0);
	}

	void CharacterState::FlipFacingDirection()
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		Sprite& sprite = ecs->GetComponentRef(Sprite, entity);

		if( sprite.canFlip )
		{
			if( sprite.flip == SDL_FLIP_HORIZONTAL)
				sprite.flip = SDL_FLIP_NONE;
			else
				sprite.flip = SDL_FLIP_HORIZONTAL;
		}
	}
	

	// PlayerController
	// ------------------------------------------------------------------
	PlayerController::PlayerController() { }
	

	// Pathing
	// ------------------------------------------------------------------
	Pathing::Pathing() : target(EntityInvalid) { }

	
	// Damage
	// ------------------------------------------------------------------
	Damage::Damage() : value(0), force(0) { }

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
		EntityCoordinator* ecs = GameData::Get().ecs;

		PushBackUnique(appliedTo, _entity);

		if(Health* health = ecs->GetComponent(Health, _entity))
			health->ApplyDamage(*this);

		if(Physics* physics = ecs->GetComponent(Physics, _entity))
		{
			float speed = physics->speed.x;

			const Transform& transform = ecs->GetComponentRef(Transform, _entity);

			const float force_direction = source.x < transform.GetObjectCenter().x ? force : -force;
			const float impulse = force_direction / physics->mass;
			physics->speed += VectorF(impulse, 0.0f);
		}
	}


	// Health
	// ------------------------------------------------------------------
	Health::Health() : maxHealth(0), currentHealth(0), invulnerable(false) { }

	void Health::Init(const SettingValues& values)
	{
		maxHealth = values.GetFloat("max_health");
		currentHealth = maxHealth;
	}

	void Health::ApplyDamage(const Damage& damage)
	{
		if(invulnerable)
			return;

		currentHealth -= damage.value;
		currentHealth = std::clamp(currentHealth, 0.0f, maxHealth);
	}
	

	// Spawner
	// ------------------------------------------------------------------
	Spawner::Spawner() : entitySpawnFn(nullptr), spawnId(nullptr), spawnConfig(nullptr) { }

	bool Spawner::Spawn(const char* spawn_id, const char* spawn_config, EntitySpawnFn spawnFn)
	{
		spawnId = spawn_id;
		spawnConfig = spawn_config;
		entitySpawnFn = spawnFn;

		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		ECS::Animator& animator = ecs->GetComponentRef(Animator, entity);
		animator.StartAnimation(ActionState::Active);
		return true;
	}

	void Spawner::Update()
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		Animator& animator = ecs->GetComponentRef(Animator, entity);

		if(animator.GetActiveAnimation().action == ActionState::Active)
		{
			if(animator.frameIndex > 1 && entitySpawnFn)
			{
				VectorF spawner_center = GetPosition(entity);

				// move the object to the spawner center
				Entity spawned_entity = entitySpawnFn(spawnId, spawnConfig);
				ECS::Transform& transform = ecs->GetComponentRef(Transform, spawned_entity);
				VectorF translation = spawner_center - transform.GetObjectCenter();
				transform.SetWorldPosition(transform.worldPosition + translation);;

				// ray cast this rect onto the floow
				RectF rect = GetRect(spawned_entity);

				float shift_y = 0.0f;
				float distance = 0.0f;
				if( RaycastToFloor(rect, distance) )
					shift_y = distance;

				transform.SetWorldPosition( transform.worldPosition + VectorF(0.0f, shift_y));

				entitySpawnFn = nullptr;
			}

			if(animator.loopCount > 0)
			{
				animator.StartAnimation(ActionState::Idle);
			}
		}
	}
	

	// Door
	// ------------------------------------------------------------------
	Door::Door() : triggerRange(0)
	{
		colliders[0] = EntityInvalid;
		colliders[1] = EntityInvalid;
	}

	void Door::Init()
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		Animator& animator = ecs->GetComponentRef(Animator, entity);
		animator.StartAnimation(ActionState::Close);
	}

	void Door::GenerateColliders(float width)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		Entity top = ecs->CreateEntity( "top door collider");
		Entity bot = ecs->CreateEntity( "bot door collider");
		ecs->AddComponent(Transform, top);
		ecs->AddComponent(Transform, bot);		
		ecs->AddComponent(Collider, top);
		ecs->AddComponent(Collider, bot);
		colliders[0] = top;
		colliders[1] = bot;

		
		ECS::EntityData::SetParent(top, entity);
		ECS::EntityData::SetParent(bot, entity);

		const Transform& door_transform = ecs->GetComponentRef(Transform, entity);
		const VectorF size(door_transform.size.x * width, door_transform.size.y * 0.5f);
		const float x_pos = door_transform.worldPosition.x + door_transform.size.x * 0.5f - size.x * 0.5f;
		const VectorF pos(x_pos, door_transform.worldPosition.y);
		
		Transform& top_transform = ecs->GetComponentRef(Transform, top);
		Transform& bot_transform = ecs->GetComponentRef(Transform, bot);
		Collider& top_collider = ecs->GetComponentRef(Collider, top);
		Collider& bot_collider = ecs->GetComponentRef(Collider, bot);

		VectorF door_part_size = VectorF(door_transform.size.x, door_transform.size.y * 0.5f);

		top_transform.size = door_part_size;
		top_transform.SetLocalPosition(VectorF(0,0));
		top_transform.InitCollider(top_collider);
		
		VectorF relative_size = VectorF(width, 1.0f);
		VectorF top_relative_pos = VectorF(0.5f - width * 0.5f, 0.0f);
		top_collider.SetRelativeRect(top_relative_pos, relative_size);

		bot_transform.size = door_part_size;
		bot_transform.SetLocalPosition(VectorF(0, door_transform.size.y * 0.5f));
		bot_transform.InitCollider(bot_collider);

		VectorF bot_relative_pos = VectorF(0.5f - width * 0.5f, 0.0f);
		bot_collider.SetRelativeRect(bot_relative_pos, relative_size);

		//top_collider.SetFlag(ECS::Collider::PlayerOnly);
		top_collider.SetFlag(ECS::Collider::IsTerrain);
		//bot_collider.SetFlag(ECS::Collider::PlayerOnly);
		bot_collider.SetFlag(ECS::Collider::IsTerrain);
	}

	void Door::Update()
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		Animator& animator = ecs->GetComponentRef(Animator, entity);
		const Animation& animation = animator.GetActiveAnimation();
	
		Entity player_entity = Player::Get();
		const VectorF player_pos = GetPosition(Player::Get());
		if(!ecs->IsAlive(player_entity))
			return;

		const VectorF door_pos = GetPosition(entity);
		const bool withing_range = std::abs(player_pos.x - door_pos.x) < triggerRange;
		if(withing_range)
		{
			if(animation.action != ActionState::Open)
			{
				const int frame_index = animator.frameIndex;
				animator.StartAnimation(ActionState::Open);
				animator.frameIndex = animation.frameCount - frame_index;
			}
		}
		else
		{
			if(animation.action != ActionState::Close)
			{
				const int frame_index = animator.frameIndex;
				animator.StartAnimation(ActionState::Close);
				animator.frameIndex = animation.frameCount - frame_index;
			}
		}

		float animation_progress = (float)animator.frameIndex / (float)(animation.frameCount - 1);
		if(animation.action == ActionState::Close)
		{
			animation_progress = 1 - animation_progress;
			if(animator.loopCount > 0)
				animation_progress = 0.0f;
		}

		const Transform& transform = ecs->GetComponentRef(Transform, entity);
		float travel_distance = transform.size.y * 0.5f;

		Transform& top_transform = ecs->GetComponentRef(Transform, colliders[0]);
		VectorF top_local_start_position = VectorF(top_transform.localPosition.x, 0.0f);
		VectorF top_local_current_position = top_local_start_position - VectorF(0, travel_distance * animation_progress);
		top_transform.SetLocalPosition(top_local_current_position);
		
		Transform& bot_transform = ecs->GetComponentRef(Transform, colliders[1]);
		VectorF bot_local_start_position = VectorF(bot_transform.localPosition.x, transform.size.y * 0.5f);
		VectorF bot_local_current_position = bot_local_start_position + VectorF(0, travel_distance * animation_progress);
		bot_transform.SetLocalPosition(bot_local_current_position);
	}

	// helpers
	Entity GetParent(Entity child)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		if(EntityData* ed = ecs->GetComponent(EntityData, child))
		{
			return ed->parent;
		}

		return EntityInvalid;
	}

	VectorF GetPosition(Entity entity)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		if(Transform* transform = ecs->GetComponent(Transform, entity))
		{
			return transform->GetObjectCenter();
		}

		return VectorF::zero();
	}

	
	RectF GetRect(Entity entity)
	{		
		EntityCoordinator* ecs = GameData::Get().ecs;
		if(const Collider* collider = ecs->GetComponent(Collider, entity))
		{
			return collider->rect;
		}
		else if(Transform* transform = ecs->GetComponent(Transform, entity))
		{
			return transform->GetRect();
		}

		return RectF();
	}

	const ObjectConfig* GetObjectConfig(Entity entity)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
		ASSERT(!state.config.empty(), "Entity %s has no object config string", ecs->entities.GetEntityName(entity));
		ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>(state.config.c_str());
		ASSERT(config != nullptr, "Entity has no object config");
		return config;
	}
}