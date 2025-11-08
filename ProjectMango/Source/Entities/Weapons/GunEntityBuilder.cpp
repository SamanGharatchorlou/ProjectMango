#include "pch.h"
#include "GunEntityBuilder.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"

#include "Graphics/Raycast.h"
#include "ECS/Components/Biome.h"
#include "Scene/SceneParsing/EntityBuilder.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/UIComponents.h"

#include "ECS/Components/GunComponents.h"
#include "Core/Helpers.h"


using namespace ECS; 

Entity CreateBasicBullet(Firearm& firearm)
{
	Entity handler = GetParent(firearm.entity);

	EntityMetaData emd;
	emd.id = firearm.magazine.bulletId;
	emd.position = GetPosition(firearm.entity);

	Entity bullet_entity = CreateBasicObject(emd);
	const Config* config = GetConfig(bullet_entity);
	
	// direction
	const Sprite& sprite = GetComponentRef(Sprite, firearm.entity);
	float rotation = sprite.rotation;
	float rad = Maths::toRadians(rotation);
	VectorF direction(cos(rad),sin(rad));
	if(sprite.IsFlipped())
		direction = direction * -1.0f;

	direction = direction.normalise();

	// start
	const Transform& transform = GetComponentRef(Transform, firearm.entity);
	const Config* firearm_config = GetConfig(firearm.entity);
	VectorF start_point = firearm_config->data.GetVectorF("barrel_end");

	const Arm& arm = GetComponentRef(Arm, firearm.entity);
	VectorF start_position = arm.GetPosition(start_point);

	Transform& bullet_transform = GetComponentRef(Transform, bullet_entity);
	bullet_transform.SetObjectCenter(start_position);

	// Physics
	Physics& physics = AddComponent(Physics, bullet_entity);
	float speed = config->data.GetFloat("speed");
	physics.speed = direction * speed;
	physics.maxSpeed = physics.speed;

	// damage
	Damage& damage = AddComponent(Damage, bullet_entity);
	damage.Init(config);
	
	// DeathScentence
	DeathScentence& death_scenetence = AddComponent(DeathScentence, bullet_entity);
	// give it a 2 second death time... just in case
	death_scenetence.deathTimer = 2.0f;

	const Level& level = Biome::GetLevel(start_position);

	std::vector<Entity> entities_to_ignore;
	entities_to_ignore.push_back(handler);
	
	std::vector<u32> no_collider_flags;
	
	RaycastResult result;
	Raycast(start_position, direction, level.size.y, result, &entities_to_ignore, &no_collider_flags);

	if(result.entity)
	{
		if(Collider* collider = GetComponent(Collider, result.entity))
		{
			death_scenetence.deathZone = GetRect(result.entity);

			if(IsEnemy(result.entity))
			{
				damage.source = start_position;
				if(damage.CanApplyTo(result.entity))
				{
					damage.ApplyTo(result.entity);
				}
			}
		}
	}

	return bullet_entity;
}


Entity EquipFirearm(Entity handler, const char* firearm_id)
{
	// drop current weapon
	if(Entity gun_entity = GetFirstChild(handler))
	{	 
		if(Firearm* firearm = GetComponent(Firearm, gun_entity))
		{
			EntityData::SetParent(gun_entity, EntityInvalid);
		}
	}

	// firearm is a child entity of the player
	EntityMetaData emd;
	emd.id = firearm_id;
	emd.position = GetPosition(handler);

	Entity firearm_entity = CreateBasicObject(emd);
	EntityData::SetParent(firearm_entity, handler);
	
	const Config* config = GetConfig(firearm_entity);

	// Firearm
	Firearm& firearm = AddComponent(Firearm, firearm_entity);

	// Transform
	Transform& transform = GetComponentRef(Transform, firearm_entity);
	RectF handler_rect = GetRect(handler);
	VectorF center_pos = handler_rect.Size() * 0.5f;

	Arm* arm = GetComponent(Arm, firearm_entity);
	if(!arm)
		arm = &AddComponent(Arm, firearm_entity);
	
	arm->anchorPoint = VectorF(0.14f, 0.52f);
	arm->target = UICursor::Get()->entity;

	// this isnt correct really, it should be the same as anchorPoint (or close to at least)
	// why does relative to top left(almost) work? shouldnt it be more like 0.5,0.5?
	float anchor_x = config->data.GetFloat("anchor_point_x");
	float anchor_y = config->data.GetFloat("anchor_point_y");

	center_pos.x = center_pos.x - transform.size.x * anchor_x;
	center_pos.y = center_pos.y - transform.size.y * anchor_y;

	transform.SetLocalPosition(center_pos);

	firearm.magazine.bulletId = config->data.GetString("defaultMagazine");

	return firearm_entity;
}