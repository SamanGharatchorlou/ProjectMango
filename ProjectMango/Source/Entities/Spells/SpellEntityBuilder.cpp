#include "pch.h"
#include "SpellEntityBuilder.h"


#include "ECS/EntityCoordinator.h"
#include "System/Files/ConfigManager.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Physics.h"
#include "Core/Helpers.h"

namespace Spell
{
	typedef ECS::Entity (*CreateEntityFn)(ECS::Entity caster, VectorF target);

	static ECS::Entity CreateBasicObject(const char* id, const char* config_id)
	{
		// find the floor
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		ECS::Entity entity = ecs->CreateEntity(id);
		ecs->AddComponent(Transform, entity);
		ecs->AddComponent(Animator, entity);
		ecs->AddComponent(Sprite, entity);

		const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>(config_id);

		// Transform
		ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);
		transform.Init(config->values, VectorF());

		// Animation
		ECS::Animator& animator = ecs->GetComponentRef(Animator, entity);
		animator.Init(config->strings.getString("animation"));
		
		if(config->values.GetBool("randomise_frame_start"))
		{
			int frame_start = (rand() % animator.GetActiveAnimation().frameCount) + 1;
			animator.frameIndex = frame_start;
		}

		if(config->values.Contains("randomise_frame_speed"))
		{
			float variation = config->values.GetFloat("randomise_frame_speed");

			int var_range = (int)(variation * 100.0f);

			int value = rand() % (int)(var_range * 2);
			float diff = (float)(value - var_range) / 100.0f;

			ECS::Animation& animation = animator.animations[animator.activeAnimation];
			animation.frameTime = animation.frameTime + (diff * animation.frameTime);
		}

		// Sprite
		ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
		sprite.renderLayer = 6;

		return entity;
	}

	ECS::Entity CreateFireball(ECS::Entity caster, VectorF target)
	{
		ECS::Entity entity = CreateBasicObject("Fireball", "FireballConfig");

		const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>("FireballConfig");

		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		ecs->AddComponent(Physics, entity);
		ecs->AddComponent(Collider, entity);
		ecs->AddComponent(Damage, entity);

		// set position to the caster position
		ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);
		VectorF caster_position = ECS::Transform::GetObjectCenter(caster);
		transform.SetWorldPositionCenter( caster_position );

		// direction
		VectorF direction = target - caster_position;
		direction = direction.normalise();

		ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
		sprite.rotation = direction.getRotation();

		// apply speed (from config) and direction
		ECS::Physics& physics = ecs->GetComponentRef(Physics, entity);
		float speed = config->values.GetFloat("speed");
		physics.speed = direction * speed;
		physics.maxSpeed = physics.speed;

		// Collider
		ECS::Collider& collider = ecs->GetComponentRef(Collider, entity);
		collider.InitFromTransform(transform);
		collider.SetFlag(ECS::Collider::IsDamage);

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
		if(s_spellEntitiyMap.contains(id))
		{
			return s_spellEntitiyMap.at(id)(caster, target);
		}

		return ECS::EntityInvalid;
	}
}