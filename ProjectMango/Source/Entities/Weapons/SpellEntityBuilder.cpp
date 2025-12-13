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
		BasicString spell_id = id;
		ECS::Entity entity = ECS::CreateEntity(spell_id.c_str(), true);

		const Config* config = ECS::GetConfigFromEntity(entity);

		// Transform
		ECS::Transform& transform = AddComponent(Transform, entity);
		transform.size = config->data.GetVectorF("size_x", "size_y");
		transform.SetWorldPosition(VectorF());
		
		// Animation
		ECS::Animator& animator = AddComponent(Animator, entity);
		animator.Init(config);

		transform.center = animator.GetActiveAnimation().objectCenter;
		
		// Collider
		ECS::Collider& collider = AddComponent(Collider, entity);		
		collider.SetFlag(ECS::Collider::IsDamage);
		collider.SetFlag(ECS::Collider::IgnorePlayer);
		collider.destroyOnContact = true;

		transform.InitCollider(collider);		
		VectorF caster_position = ECS::Transform::GetObjectCenter(caster);
		transform.SetObjectCenter( caster_position );

		// Sprite
		ECS::Sprite& sprite = AddComponent(Sprite, entity);
		sprite.renderLayer = ECS::RenderLayer::Spell;

		// Damage
		ECS::Damage& damage = AddComponent(Damage, entity);
		damage.value = config->data.GetFloat("damage");

		// Spell
		ECS::Spell& spell = AddComponent(Spell, entity);
		spell.name = id;
		spell.target = target;
		spell.caster = caster;

		return entity;
	}

	ECS::Entity CreateFireball(ECS::Entity caster, VectorF target)
	{
		const char* id = "Fireball";
		ECS::Entity entity = CreateBasicSpell(id, caster, target);
		const Config* config = ECS::GetConfigFromEntity(entity);
		
		// direction
		VectorF caster_position = ECS::Transform::GetObjectCenter(caster);
		VectorF direction = target - caster_position;
		direction = direction.normalise();

		// Physics
		ECS::Physics& physics = AddComponent(Physics, entity);
		float speed = config->data.GetFloat("speed");
		physics.speed = direction * speed;
		physics.maxSpeed = physics.speed;

		// Sprite
		ECS::Sprite& sprite = GetComponentRef(Sprite, entity);
		sprite.rotation = direction.getRotation();

		return entity;
	}

	ECS::Entity CreateLightning(ECS::Entity caster, VectorF target)
	{
		const char* id = "Lightning";
		ECS::Entity entity = CreateBasicSpell(id, caster, target);

		// collder
		ECS::Collider& collider = AddComponent(Collider, entity);
		collider.destroyOnContact = false;

		// direction
		VectorF caster_position = ECS::Transform::GetObjectCenter(caster);
		VectorF direction = target - caster_position;
		direction = direction.normalise();

		// sprite
		ECS::Sprite& sprite = GetComponentRef(Sprite, entity);
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