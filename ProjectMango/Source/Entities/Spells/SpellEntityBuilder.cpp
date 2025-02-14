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

	static ECS::Entity CreateBasicSpell(const char* id, ECS::Entity caster, VectorF target)
	{
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;

		BasicString spell_id = id;
		ECS::Entity entity = ECS::CreateEntity(spell_id.c_str(), true);

		const ObjectConfig* config = ECS::GetObjectConfig(entity);

		// Transform
		ECS::Transform& transform = ecs->AddComponent(Transform, entity);
		transform.Init(config, VectorF());
		
		// Animation
		ECS::Animator& animator = ecs->AddComponent(Animator, entity);
		animator.Init(config);

		transform.center = animator.GetActiveAnimation().objectCenter;
		
		// Collider
		ECS::Collider& collider = ecs->AddComponent(Collider, entity);		
		collider.SetFlag(ECS::Collider::IsDamage);
		collider.SetFlag(ECS::Collider::IgnorePlayer);
		collider.destroyOnContact = true;

		transform.InitCollider(collider);		
		VectorF caster_position = ECS::Transform::GetObjectCenter(caster);
		transform.SetWorldPositionCenter( caster_position );

		// Sprite
		ECS::Sprite& sprite = ecs->AddComponent(Sprite, entity);
		sprite.renderLayer = 4;

		// Damage
		ECS::Damage& damage = ecs->AddComponent(Damage, entity);
		damage.value = config->values.GetFloat("damage");

		// Spell
		ECS::Spell& spell = ecs->AddComponent(Spell, entity);
		spell.name = id;
		spell.target = target;
		spell.caster = caster;

		return entity;
	}

	ECS::Entity CreateFireball(ECS::Entity caster, VectorF target)
	{
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;

		const char* id = "Fireball";
		ECS::Entity entity = CreateBasicSpell(id, caster, target);
		const ObjectConfig* config = ECS::GetObjectConfig(entity);
		
		// direction
		VectorF caster_position = ECS::Transform::GetObjectCenter(caster);
		VectorF direction = target - caster_position;
		direction = direction.normalise();

		// Physics
		ECS::Physics& physics = ecs->AddComponent(Physics, entity);
		float speed = config->values.GetFloat("speed");
		physics.speed = direction * speed;
		physics.maxSpeed = physics.speed;

		// Sprite
		ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
		sprite.rotation = direction.getRotation();

		return entity;
	}

	ECS::Entity CreateLightning(ECS::Entity caster, VectorF target)
	{
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;

		const char* id = "Lightning";
		ECS::Entity entity = CreateBasicSpell(id, caster, target);

		// collder
		ECS::Collider& collider = ecs->AddComponent(Collider, entity);
		collider.destroyOnContact = false;

		// direction
		VectorF caster_position = ECS::Transform::GetObjectCenter(caster);
		VectorF direction = target - caster_position;
		direction = direction.normalise();

		// sprite
		ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
		sprite.rotation = direction.getRotation();

		// turn into quad collider
		collider.SetFlag(ECS::Collider::QuadCollider);

		return entity;
	}

	static std::unordered_map<BasicString, CreateEntityFn> s_spellEntitiyMap;
	
	void CreateEntityMap()
	{
		if(s_spellEntitiyMap.size() == 0)
		{
			s_spellEntitiyMap["Fireball"] = CreateFireball;
			s_spellEntitiyMap["Lightning"] = CreateLightning;
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