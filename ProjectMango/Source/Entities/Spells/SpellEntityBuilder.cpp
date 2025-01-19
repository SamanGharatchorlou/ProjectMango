#include "pch.h"
#include "SpellEntityBuilder.h"


#include "ECS/EntityCoordinator.h"
#include "System/Files/ConfigManager.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/SpellComponents.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Physics.h"
#include "Core/Helpers.h"

namespace Magic
{
	typedef ECS::Entity (*CreateEntityFn)(ECS::Entity caster, VectorF target);

	static ECS::Entity CreateBasicSpell(const char* id, const char* config_id)
	{
		// find the floor
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		ECS::Entity entity = ecs->CreateEntity(id);
		ecs->AddComponent(Transform, entity);
		ecs->AddComponent(Animator, entity);
		ecs->AddComponent(Sprite, entity);
		ecs->AddComponent(Spell, entity);

		const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>(config_id);

		// Transform
		ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);
		transform.Init(config->values, VectorF());

		// Animation
		ECS::Animator& animator = ecs->GetComponentRef(Animator, entity);
		animator.Init(config);

		transform.center = animator.GetActiveAnimation().objectCenter;

		// Sprite
		ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
		sprite.renderLayer = 6;

		return entity;
	}

	ECS::Entity CreateFireball(ECS::Entity caster, VectorF target)
	{
		ECS::Entity entity = CreateBasicSpell("Fireball", "FireballConfig");

		const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>("FireballConfig");

		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		ecs->AddComponent(Physics, entity);
		ecs->AddComponent(Collider, entity);
		ecs->AddComponent(Damage, entity);

		// set position to the caster position
		ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);
		ECS::Collider& collider = ecs->GetComponentRef(Collider, entity);
		transform.InitCollider(collider);

		VectorF caster_position = ECS::Transform::GetObjectCenter(caster);
		transform.SetWorldPositionCenter( caster_position );
		
		// Collider
		collider.SetFlag(ECS::Collider::IsDamage);
		collider.SetFlag(ECS::Collider::IgnorePlayer);
		//collider.destroyOnContact = true;
		collider.reboundCount = 3;

		// direction
		VectorF direction = target - caster_position;
		direction = direction.normalise();

		ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
		sprite.rotation = direction.getRotation();

		// Physics
		ECS::Physics& physics = ecs->GetComponentRef(Physics, entity);
		float speed = config->values.GetFloat("speed");
		physics.speed = direction * speed;
		physics.maxSpeed = physics.speed;

		// Damage
		ECS::Damage& damage = ecs->GetComponentRef(Damage, entity);
		damage.value = config->values.GetFloat("damage");

		return entity;
	}

	static std::unordered_map<BasicString, CreateEntityFn> s_spellEntitiyMap;
	
	void CreateEntityMap()
	{
		if(s_spellEntitiyMap.size() == 0)
		{
			s_spellEntitiyMap["Fireball"] = CreateFireball;
		}
	}
	
	ECS::Entity GetNewEntity(const char* id, ECS::Entity caster, VectorF target)
	{
		if(SpellExists(id))
		{
			return s_spellEntitiyMap.at(id)(caster, target);
		}

		return ECS::EntityInvalid;
	}

	bool SpellExists(const char* spell_name)
	{
		if(spell_name)
			return s_spellEntitiyMap.contains(spell_name);

		return false;
	}
}