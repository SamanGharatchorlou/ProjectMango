#include "pch.h"
#include "Components.h"

#include "Animations/CharacterStates.h"
#include "Core/Helpers.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Biome.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Physics.h"
#include "ECS/EntityCommon.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/EntSystems/TransformSystem.h"
#include "Entities/Player/PlayerCharacter.h"
#include "Entities/Weapons/PickupCallbacks.h"
#include "Graphics/TextureManager.h"
#include "System/Files/ConfigManager.h"

#include "Graphics/RenderManager.h"
#include "ECS/EntSystems/RenderSystem.h"

namespace ECS
{
	Entity CreateEntity(const ECS::EntityMetaData& emd)
	{
		Entity entity = ecs->CreateNewEntity();
		if (!emd.id.empty()) 
		{ 
			EntityData& ed = AddComponent(EntityData, entity); 
			ed.id = emd.id; 
			ed.subType = emd.type; 
		}
		return entity;
	}

	Entity CreateEntity(const char* id, bool config_postfix)
	{
		Entity entity = ecs->CreateNewEntity();
		if (id)
		{
			EntityData& ed = AddComponent(EntityData, entity);
			ed.id = id;
			//if (config_postfix)
			//{
			//	char buffer[64];
			//	snprintf(buffer, 64, "%sConfig", id);
			//	ed.config = buffer;
			//}
		}

		return entity;
	}

	//Entity CreateEntity(const EntityMetaData& emd)
	//{
	//	return CreateEntity(emd.id.c_str(), emd.idPostfix.c_str());
	//}

	const char* GetName(Entity entity)
	{
		if(EntityData* ed = GetComponent(EntityData, entity))
			return ed->id.c_str();

		return nullptr;
	}

	const Config* GetConfig(Entity entity)
	{
		const Config* config = nullptr;
		if (const EntityData* ed = GetComponent(EntityData, entity))
		{

			config = ConfigManager::Get()->GetConfig(ed->id.c_str());

			//if (ed->config.empty())
			//{
			//	DebugPrint(Warning, "Entity: '%s' has no config string in EntityData",
			//		ed->id.empty() ? "No ID" : ed->id.c_str());

			//	return nullptr;
			//}

			//config = ConfigManager::Get()->GetConfig(ed->config.c_str());
			//if (!config)
			//{
			//	DebugPrint(Warning, "No config found for entity '%s' with config ID '%s'",
			//		ed->id.empty() ? "No ID" : ed->id.c_str(), ed->config.c_str());
			//}
		}

		return config;
	}


	//const Config* GetConfigFromID(const char* id)
	//{
	//	char buffer[64];
	//	snprintf(buffer, 64, "%sConfig", id);
	//	return ConfigManager::Get()->GetConfig(buffer);
	//}

	// EntityData
	// ------------------------------------------------------------------
	EntityData::EntityData() : 
		parent(EntityInvalid)
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

	void Transform::Init(const Config* config, VectorF pos)
	{
		if(config)
		{
			size = config->data.GetVectorF("size_x", "size_y");
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

	void Transform::InitCollider(Collider& collider)
	{
		if(!size.isPositive())
			DebugPrint(Warning, "Transform size has to be set before init'ing collider");

		collider.InitFromTransform(*this);
	}
	
	void Transform::Init(const Config* config, VectorF pos, Collider& collider)
	{
		Init(config, pos);
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
			collider->UpdateFromTransform(this);
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
		renderLayer(RenderLayer::None)
	{ }

	void Sprite::Init(const Config* config)
	{
		if(config)
		{
			SetTexture(config->data.GetString("sprite"));
			canFlip = config->data.GetBool("can_flip", true);
		}
	}


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

	void CharacterState::Init(const Config* config)
	{
		if(config)
		{
			isRanged = config->data.GetBool("ranged", true);
			isMelee = !isRanged;
		}
	}	

	// PlayerController
	// ------------------------------------------------------------------
	PlayerController::PlayerController() { }
	

	// Pathing
	// ------------------------------------------------------------------
	Pathing::Pathing() /*: target(EntityInvalid)*/ { }

	void Pathing::Init()
	{
		VectorF pos = GetPosition(entity);
		const Level& level = Biome::GetLevel(pos);
		levelIndex = level.index;
	}
	
	// Damage
	// ------------------------------------------------------------------
	Damage::Damage() : value(0), force(0) 
	{ }

	
	void Damage::Init(const Config* config)
	{
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
			health->ApplyDamage(*this);

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

	void Health::Init(const Config* config)
	{
		maxHealth = config->data.GetFloat("max_health");
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
	Spawner::Spawner() : entitySpawnFn(nullptr), spawnId(nullptr) { }

	bool Spawner::Spawn(const char* spawn_id, EntitySpawnFn spawnFn)
	{
		spawnId = spawn_id;
		entitySpawnFn = spawnFn;

		ECS::Animator& animator = GetComponentRef(Animator, entity);
		animator.StartAnimation(ActionState::Active);
		return true;
	}

	void Spawner::Update()
	{
		Animator& animator = GetComponentRef(Animator, entity);

		if(animator.GetActiveAnimation().action == ActionState::Active)
		{
			if(animator.frameIndex > 1 && entitySpawnFn)
			{
				VectorF spawner_center = GetPosition(entity);

				// move the object to the spawner center
				ECS::EntityMetaData emd;
				emd.id = spawnId;

				Entity spawned_entity = entitySpawnFn(emd);
				ECS::Transform& transform = GetComponentRef(Transform, spawned_entity);
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
		Animator& animator = GetComponentRef(Animator, entity);
		animator.StartAnimation(ActionState::Close);
	}

	void Door::GenerateColliders(float width)
	{
		Entity top = CreateEntity("top door collider");
		Entity bot = CreateEntity("bot door collider");
		AddComponent(Transform, top);
		AddComponent(Transform, bot);		
		AddComponent(Collider, top);
		AddComponent(Collider, bot);
		colliders[0] = top;
		colliders[1] = bot;

		ECS::EntityData::SetParent(top, entity);
		ECS::EntityData::SetParent(bot, entity);

		const Transform& door_transform = GetComponentRef(Transform, entity);
		const VectorF size(door_transform.size.x * width, door_transform.size.y * 0.5f);
		const float x_pos = door_transform.worldPosition.x + door_transform.size.x * 0.5f - size.x * 0.5f;
		const VectorF pos(x_pos, door_transform.worldPosition.y);
		
		Transform& top_transform = GetComponentRef(Transform, top);
		Transform& bot_transform = GetComponentRef(Transform, bot);
		Collider& top_collider = GetComponentRef(Collider, top);
		Collider& bot_collider = GetComponentRef(Collider, bot);

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
		Animator& animator = GetComponentRef(Animator, entity);
		const Animation& animation = animator.GetActiveAnimation();
	
		Entity player_entity = Player::Get();
		const VectorF player_pos = GetPosition(Player::Get());
		if(!ecs->IsAlive(player_entity))
			return;

		const RectF door_rect = GetRect(entity);
		VectorF door_pos = door_rect.Center();

		const bool withing_range_x = std::abs(player_pos.x - door_pos.x) < triggerRange;
		const bool withing_range_z = std::abs(player_pos.y - door_pos.y) < door_rect.Height();
		if(withing_range_x && withing_range_z)
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

		const Transform& transform = GetComponentRef(Transform, entity);
		float travel_distance = transform.size.y * 0.5f;

		Transform& top_transform = GetComponentRef(Transform, colliders[0]);
		VectorF top_local_start_position = VectorF(top_transform.localPosition.x, 0.0f);
		VectorF top_local_current_position = top_local_start_position - VectorF(0, travel_distance * animation_progress);
		top_transform.SetLocalPosition(top_local_current_position);
		
		Transform& bot_transform = GetComponentRef(Transform, colliders[1]);
		VectorF bot_local_start_position = VectorF(bot_transform.localPosition.x, transform.size.y * 0.5f);
		VectorF bot_local_current_position = bot_local_start_position + VectorF(0, travel_distance * animation_progress);
		bot_transform.SetLocalPosition(bot_local_current_position);
	}


	// Pickup
	// ------------------------------------------------------------------
	Pickup::Pickup() : pickedUp(false) { }

	void Pickup::Update()
	{
		if (!pickedUp)
		{
			if (Collider* collider = GetComponent(Collider, entity))
			{
				if (collider->HasCollided())
				{
					Entity hit_entity = collider->collisions.front();

					PickUps::OnPickupFn fn = PickUps::GetCallback(itemId.c_str());
					if (fn && fn(entity, hit_entity))
					{
						// only trigger once
						//onPickupFn = nullptr;
						pickedUp = true;
					}
				}
			}
		}
	}

	// DeathScentence
	// ------------------------------------------------------------------
	DeathScentence::DeathScentence() : deathTimer(-FLT_MAX), deathZone(InvalidRectF) { }

	void DeathScentence::Update(float dt)
	{
		if(deathTimer != -FLT_MAX)
		{
			deathTimer -= dt;
		}

		if(canDie)
		{
			if(deathTimer != -FLT_MAX)
			{
				if(deathTimer < 0)
				{
					ecs->entities.KillEntity(entity);
					return;
				}

			}

			if( deathZone.isValid() )
			{
				if(Contains(deathZone, GetPosition(entity)))
				{
					ecs->entities.KillEntity(entity);
					return;
				}
			}
		}
	}

	// Arm
	// ------------------------------------------------------------------
	Arm::Arm() : anchorPoint( VectorF(0.5f, 0.5f)), target(EntityInvalid) { }

	void Arm::Update()
	{
		if(ecs->IsAlive(target))
		{
			// make sure we're all up to date with parent and child positions
			TransformSystem::UpdateChildrenTransforms(GetParent(entity));

			if(const Transform* transform = GetComponent(Transform, entity))
			{
				if(Sprite* sprite = GetComponent(Sprite, entity))
				{
					// direction
					VectorF caster_position = transform->GetObjectCenter();
					// could be center, but realistically its the cursor thats the target, and we want the top left
					// hmmmm will this be a problem for when we dont was it to be the cursor and not the top left?
					VectorF direction = GetRect(target).TopLeft() - caster_position;
					direction = direction.normalise();
					
					if(sprite->IsFlipped())
					{
						sprite->flipPoint = (VectorF(1.0f,1.0f) - anchorPoint);
						
						direction = direction * -1;
					}
					else
					{
						sprite->flipPoint = anchorPoint;
					}
					
					float rotation = direction.getRotation();

					// handle cursor behind the player, mirror the rotation
					if(!IsTargetInFrontOfSource(target, entity))
					{
						direction.y = -direction.y;
						rotation = direction.getRotation();

						rotation += 180.0f;
					}
								
					// we live between
					if(rotation > 180)
						rotation -= 360;

					rotation = Maths::clamp(rotation, -45.0f, 45.0f);
					sprite->rotation = rotation;
				}
			}
		}
	}

	VectorF Arm::GetPosition(VectorF relative_posision) const
	{
		Sprite* sprite = GetComponent(Sprite, entity);
		Transform* transform = GetComponent(Transform, entity);

		if(sprite && transform)
		{
			relative_posision = sprite->IsFlipped() ? VectorF(1.0f,1.0f) - relative_posision : relative_posision;

			VectorF initial_position = transform->GetRelativePosition(relative_posision);
			VectorF about_point = transform->GetRelativePosition(sprite->flipPoint);

			return initial_position.rotateVector(sprite->rotation, about_point);
		}

		return c_invalidVector;
	}

	// helpers
	// ------------------------------------------------------------------
	Entity GetParent(Entity child)
	{
		if(EntityData* ed = GetComponent(EntityData, child))
		{
			return ed->parent;
		}

		return EntityInvalid;
	}

	Entity GetFirstChild(Entity parent)
	{
		if(EntityData* ed = GetComponent(EntityData, parent))
		{
			if(ed->children.size() > 0)
				return ed->children.front();
		}

		return EntityInvalid;
	}

	VectorF GetPosition(Entity entity)
	{	
		if(Transform* transform = GetComponent(Transform, entity))
		{
			return transform->GetObjectCenter();
		}

		return VectorF::zero();
	}

	RectF GetRect(Entity entity)
	{		
		if(const Collider* collider = GetComponent(Collider, entity))
		{
			return collider->rect;
		}
		else if(Transform* transform = GetComponent(Transform, entity))
		{
			return transform->GetRect();
		}

		return RectF();
	}

	bool GetRotationParams(Entity entity, VectorF& out_aboutPoint, float& out_rotation)
	{
		if (const Transform* transform = GetComponent(Transform, entity))
		{
			if (const ECS::Sprite* sprite = GetComponent(Sprite, entity))
			{
				out_rotation = sprite->rotation;

				RectF rect = transform->GetRect();
				out_aboutPoint = rect.TopLeft() + (rect.Size() * sprite->flipPoint);
				return true;
			}
		}

		return false;
	}

	SDL_RendererFlip GetFacingDirection(Entity entity)
	{
		Sprite& sprite = GetComponentRef(Sprite, entity);
		return sprite.flip;
	}

	VectorI GetFacingDirectionVector(Entity entity)
	{
		return FacingDirectionToVector(GetFacingDirection(entity));
	}

	VectorI FacingDirectionToVector(SDL_RendererFlip facing)
	{
		int direction = (facing == SDL_FLIP_HORIZONTAL) ? -1 : 1;
		return VectorI(direction, 0);
	}

	void SetFacingDirection(Entity entity, SDL_RendererFlip direction)
	{
		Sprite& sprite = GetComponentRef(Sprite, entity);

		if (sprite.canFlip)
			sprite.flip = direction;
	}

	void FlipFacingDirection(Entity entity)
	{
		Sprite& sprite = GetComponentRef(Sprite, entity);

		if (sprite.canFlip)
		{
			if (sprite.flip == SDL_FLIP_HORIZONTAL)
				sprite.flip = SDL_FLIP_NONE;
			else
				sprite.flip = SDL_FLIP_HORIZONTAL;
		}
	}

	SDL_RendererFlip GetDesiredFacingDirection(Entity entity, Entity target_entity)
	{
		VectorF self = GetPosition(entity);
		VectorF target = GetPosition(target_entity);
		return (target.x > self.x) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
	}
}