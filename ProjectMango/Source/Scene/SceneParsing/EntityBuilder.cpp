#include "pch.h"
#include "EntityBuilder.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "System/Files/ConfigManager.h"
#include "ECS/Components/Biome.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Collider.h"
#include "Graphics/Raycast.h"
#include "Entities/Enemies/ShockSweeperEnemy.h"
#include "Entities/Enemies/BlindingSpiderEnemy.h"
#include "ECS/Components/UIComponents.h"
#include "Input/InputManager.h"

typedef ECS::Entity (*CreateEntityFn)(const ECS::EntityMetaData&);

static ECS::Entity CreateBasicObject(const ECS::EntityMetaData& emd)
{
	ECS::Entity entity = ECS::CreateEntity(emd.id.c_str(), emd.ConfigId().c_str());
	if (const Config* config = ECS::GetConfig(entity))
	{
		// Transform
		ECS::Transform& transform = AddComponent(Transform, entity);
		VectorF pos = emd.position - (transform.size / 2.0f);
		transform.Init(config, pos);

		// Sprite
		ECS::Sprite& sprite = AddComponent(Sprite, entity);
		sprite.renderLayer = 6;
		sprite.canFlip = false;
		sprite.Init(config);
	}

	return entity;
}

static ECS::Entity CreateAnimatedObject(const ECS::EntityMetaData& emd)
{
	ECS::Entity entity = CreateBasicObject(emd);
	const Config* config = ECS::GetConfig(entity);

	// Animation
	ECS::Animator& animator = AddComponent(Animator, entity);
	animator.Init(config);

	return entity;
}

static ECS::Entity CreatePlayerSpawner(const ECS::EntityMetaData& emd)
{
	ECS::Entity entity = CreateAnimatedObject(emd);

	// Spawner
	AddComponent(Spawner, entity);

	return entity;
}

static ECS::Entity CreateFlower(const ECS::EntityMetaData& emd)
{
	return CreateAnimatedObject(emd);
}

static ECS::Entity CreateTorch(const ECS::EntityMetaData& emd)
{
	return CreateAnimatedObject(emd);
}

static ECS::Entity CreateDoor(const ECS::EntityMetaData& emd)
{
	ECS::Entity entity = CreateAnimatedObject(emd);

	AddComponent(Door, entity);

	const ECS::Level& level = ECS::Biome::GetLevel(entity);	
	std::vector<u32> collider_flags;
	collider_flags.push_back(ECS::Collider::IsTerrain);

	RaycastResult down_result;
	Raycast(emd.position, VectorF(0.0, 1.0f), level.size.y, down_result, nullptr, &collider_flags);

	RaycastResult up_result;
	Raycast(emd.position, VectorF(0.0, -1.0f), level.size.y, up_result, nullptr, &collider_flags);

	// no valid door position
	if(!down_result.hasHit || !up_result.hasHit)
	{
		ecs->entities.KillEntity(entity);
		return ECS::EntityInvalid;
	}

	// Door
	ECS::Door& door = GetComponentRef(Door, entity);
	door.Init();

	const Config* config = ConfigManager::Get()->GetConfig(emd.ConfigId().c_str());
	door.triggerRange = config->values.GetFloat("trigger_range");

	// Transform - sandwich the door between the top and bottom raycast points
	ECS::Transform& transform = GetComponentRef(Transform, entity);

	const VectorF top = up_result.hitPosition;
	const VectorF bot = down_result.hitPosition;
	const float height = bot.y - top.y;
	const float size_ratio = height /  transform.size.y;

	// update the transform positions
	transform.size = transform.size * size_ratio;
	transform.SetWorldPosition(top);

	door.GenerateColliders(config->values.GetFloat("collider_width"));
	
	return entity;
}

ECS::Entity CreateCursor()
{
	ECS::Entity entity = ECS::CreateEntity("Cursor", true);

	const Config* config = ECS::GetConfig(entity);

	// Transform
	ECS::Transform& transform = AddComponent(Transform, entity);
	transform.Init(config, VectorF());

	// Sprite
	ECS::Sprite& sprite = AddComponent(Sprite, entity);
	sprite.renderLayer = 9;
	sprite.canFlip = false;
	sprite.Init(config);
		
	ECS::UICursor& cursor = AddComponent(UICursor, entity);
	InputManager* input = InputManager::Get();
	cursor.cursor = &input->mCursor;

	return entity;
}

ECS::Entity CreatePickup(const ECS::EntityMetaData& emd)
{
	ECS::Entity entity = CreateBasicObject( emd );

	ECS::Pickup& pick_up = AddComponent(Pickup, entity);
	pick_up.itemId = emd.idPostfix;
	pick_up.config = emd.ConfigId();
	//pick_up.onPickupFn = ApplyReboundRune;

	ECS::Collider& collider = AddComponent(Collider, entity);
	collider.SetFlag(ECS::Collider::PlayerOnly);
	collider.SetFlag(ECS::Collider::GhostCollider);
	collider.destroyOnContact = true;

	ECS::Transform& transform = GetComponentRef(Transform, entity);
	transform.InitCollider(collider);

	return entity;
}

void CreateEntities(ECS::Entity& biome_entity)
{
	srand ((u32)time(NULL));

	// map entities
	std::unordered_map<BasicString, CreateEntityFn> CreateEntitiyFunctions;
	CreateEntitiyFunctions["PlayerSpawner"] = CreatePlayerSpawner;
	CreateEntitiyFunctions["Flower"] = CreateFlower;
	CreateEntitiyFunctions["Torch"] = CreateTorch;
	CreateEntitiyFunctions["Door"] = CreateDoor;
	CreateEntitiyFunctions["BlindingSpider"] = BlindingSpider::Create;
	CreateEntitiyFunctions["ShockSweeper"] = ShockSweeper::Create;
	CreateEntitiyFunctions["TrainingDummy"] = TrainingDummy::Create;
	CreateEntitiyFunctions["Pickup"] = CreatePickup;

	ECS::Biome& biome = GetComponentRef(Biome, biome_entity);
	for (u32 i = 0; i < biome.levels.size(); i++)
	{
		const ECS::Level& level = biome.levels[i];
		for (auto iter = level.entities.begin(); iter != level.entities.end(); iter++)
		{
			const char* entity_id = iter->first.c_str();

			if(CreateEntitiyFunctions.contains(entity_id))
			{
				CreateEntityFn create_fn = CreateEntitiyFunctions.at(entity_id);

				const std::vector<ECS::EntityMetaData>& entitiy_meta_data = iter->second;
				for( u32 e = 0; e < entitiy_meta_data.size(); e++ )
				{
					create_fn(entitiy_meta_data[e]);
				}
			}
			else
			{
				DebugPrint(Warning, "No CreateEntity function defined for %s", entity_id);
			}
		}
	}
}