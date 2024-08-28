#include "pch.h"
#include "Components.h"

#include "ECS/EntityCoordinator.h"
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

		EntityCoordinator* ecs = GameData::Get().ecs;

		for( u32 i = 0; i < ignoredDamaged.size(); i++ )
		{
			// ignore damage from this source
			if(ignoredDamaged[i] == damage.entity)
				return;
		}

		currentHealth -= damage.value;
		currentHealth = std::clamp(currentHealth, 0.0f, maxHealth);

		// todo: clean this list when the entity is dead
		ignoredDamaged.push_back(damage.entity);
	}
}