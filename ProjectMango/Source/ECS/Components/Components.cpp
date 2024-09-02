#include "pch.h"
#include "Components.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Physics.h"
#include "Core/Helpers.h"

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
	void Transform::SetLocalPosition(VectorF pos)
	{
		localPosition = pos;
		
		EntityCoordinator* ecs = GameData::Get().ecs;

		// must have a parent by this point
		EntityData& entity_data = ecs->GetComponentRef(EntityData, entity);
		Transform& parent_transform = ecs->GetComponentRef(Transform, entity_data.parent);
		SetWorldPosition(parent_transform.worldPosition + localPosition);
	}
	
	// Health
	void Health::ApplyDamage(const Damage& damage)
	{
		if(invulnerable)
			return;

		currentHealth -= damage.value;
		currentHealth = std::clamp(currentHealth, 0.0f, maxHealth);
	}

	// Damage
	bool Damage::CanApplyTo(Entity entity) const
	{
		for( u32 i = 0; i < appliedTo.size(); i++ )
		{
			// dont re-apply damage to this source
			if(appliedTo[i] == entity)
				return false;
		}

		return true;
	}

	void Damage::ApplyTo(Entity entity)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		PushBackUnique(appliedTo, entity);

		if(Health* health = ecs->GetComponent(Health, entity))
			health->ApplyDamage(*this);

		if(Physics* physics = ecs->GetComponent(Physics, entity))
		{
			VectorF impulse = force / physics->mass;
			physics->speed += impulse;
		}

		DebugPrint(PriorityLevel::Log, "Apply damage to %d", entity);
	}

	// CharacterState
	VectorI CharacterState::GetFacingDirection() const
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		Sprite& sprite = ecs->GetComponentRef(Sprite, entity);

		int direction = sprite.IsFlipped() ? -1 : 1;

		return VectorI(direction, 0);
	}
}