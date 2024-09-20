#include "pch.h"
#include "Components.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Physics.h"
#include "Core/Helpers.h"
#include "ECS/Components/Collider.h"
#include "ECS/EntSystems/TransformSystem.h"

namespace ECS
{
	// EntityData
	void EntityData::SetParent(Entity entity, Entity parent)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		// set new entity parent
		EntityData* entity_data = ecs->GetComponent(EntityData, entity);
		if(!entity_data)
			entity_data = &ecs->AddComponent(EntityData, entity);

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
			parent_entity_data = &ecs->AddComponent(EntityData, parent);

		PushBackUnique(parent_entity_data->children, entity);
	}

	// Transform
	void Transform::Init(const SettingValues& values)
	{
		size = VectorF(values.at("size_x"), values.at("size_y"));
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

	VectorF Transform::GetObjectCenter() const
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		if(const Collider* collider = ecs->GetComponent(Collider, entity))
		{
			return collider->rect.Center();
		}
		else
		{
			return worldPosition;
		}

		return VectorF::zero();
	}
	
	// Health
	void Health::Init(const SettingValues& values)
	{
		maxHealth = values.at("max_health");
		currentHealth = maxHealth;
	}

	void Health::ApplyDamage(const Damage& damage)
	{
		if(invulnerable)
			return;

		currentHealth -= damage.value;
		currentHealth = std::clamp(currentHealth, 0.0f, maxHealth);
	}

	// Damage
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

	// CharacterState
	VectorI CharacterState::GetFacingDirection() const
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		Sprite& sprite = ecs->GetComponentRef(Sprite, entity);

		int direction = sprite.IsFlipped() ? -1 : 1;

		return VectorI(direction, 0);
	}

	bool Spawner::Spawn()
	{
		return true;
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
}