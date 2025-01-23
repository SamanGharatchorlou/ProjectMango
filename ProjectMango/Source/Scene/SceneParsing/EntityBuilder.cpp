#include "pch.h"
#include "EntityBuilder.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "System/Files/ConfigManager.h"
#include "ECS/Components/Biome.h"
#include "ECS/Components/Animator.h"
#include "Core/Helpers.h"
#include "ECS/Components/Collider.h"
#include "Graphics/Raycast.h"
#include "Entities/Enemies/ShockSweeperEnemy.h"
#include "Entities/Enemies/BlindingSpiderEnemy.h"
#include "ECS/Components/UIComponents.h"
#include "Input/InputManager.h"
#include "Entities/Spells/PickupCallbacks.h"

typedef ECS::Entity (*CreateEntityFn)( const char* id, const char* config, VectorF spawn_pos );

static ECS::Entity CreateBasicObject(const char* id, const char* config_id, VectorF spawn_pos)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Entity entity = ecs->CreateEntity(id);
	if (const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>(config_id))
	{
		// Transform
		ECS::Transform& transform = ecs->AddComponent(Transform, entity);
		VectorF pos = spawn_pos - (transform.size / 2.0f);
		transform.Init(config->values, pos);

		// Sprite
		ECS::Sprite& sprite = ecs->AddComponent(Sprite, entity);
		sprite.renderLayer = 6;
		sprite.canFlip = false;

		if(config->strings.Contains("sprite"))
		{	
			sprite.SetTexture(config->strings["sprite"]);
		}
	}

	return entity;
}

static ECS::Entity CreateAnimatedObject(const char* id, const char* config_id, VectorF spawn_pos)
{
	ECS::Entity entity = CreateBasicObject(id, config_id, spawn_pos);

	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>(config_id);

	// Animation
	ECS::Animator& animator = ecs->AddComponent(Animator, entity);
	animator.Init(config);

	return entity;
}

static ECS::Entity CreatePickup(const char* id, const char* config_id, VectorF spawn_pos)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Entity entity = CreateBasicObject(id, config_id, spawn_pos);

	// Pickup
	ECS::Pickup& pickup = ecs->AddComponent(Pickup, entity);

	return entity;
}

static ECS::Entity CreatePlayerSpawner(const char* id, const char* config_id, VectorF spawn_pos)
{
	ECS::Entity entity = CreateAnimatedObject(id, config_id, spawn_pos);

	// Spawner
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ecs->AddComponent(Spawner, entity);

	return entity;
}

static ECS::Entity CreateFlower(const char* id, const char* config_id, VectorF spawn_pos)
{
	return CreateAnimatedObject(id, config_id, spawn_pos);
}

static ECS::Entity CreateTorch(const char* id, const char* config_id, VectorF spawn_pos)
{
	return CreateAnimatedObject(id, config_id, spawn_pos);
}

static ECS::Entity CreateDoor(const char* id, const char* config_id, VectorF spawn_pos)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Entity entity = CreateAnimatedObject(id, config_id, spawn_pos);

	ecs->AddComponent(Door, entity);

	const ECS::Level& level = ECS::Biome::GetLevel(entity);	
	std::vector<u32> collider_flags;
	collider_flags.push_back(ECS::Collider::IsTerrain);

	RaycastResult down_result;
	Raycast(spawn_pos, VectorF(0.0, 1.0f), level.size.y, down_result, nullptr, &collider_flags);

	RaycastResult up_result;
	Raycast(spawn_pos, VectorF(0.0, -1.0f), level.size.y, up_result, nullptr, &collider_flags);

	// no valid door position
	if(!down_result.hasHit || !up_result.hasHit)
	{
		ecs->entities.KillEntity(entity);
		return ECS::EntityInvalid;
	}

	// Door
	ECS::Door& door = ecs->GetComponentRef(Door, entity);
	door.Init();

	const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>(config_id);
	door.triggerRange = config->values.GetFloat("trigger_range");

	// Transform - sandwich the door between the top and bottom raycast points
	ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);

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
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Entity entity = CreateBasicObject( "cursor", "CursorConfig", VectorF() );

	ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
	sprite.renderLayer = 9;
		
	ECS::UICursor& cursor = ecs->AddComponent(UICursor, entity);
	InputManager* input = InputManager::Get();
	cursor.cursor = &input->mCursor;

	return entity;
}

ECS::Entity CreateRune(const char* id, const char* config_id, VectorF spawn_pos)
{	
	ECS::Entity entity = CreateBasicObject( id, config_id, spawn_pos );

	ECS::EntityCoordinator* ecs = GameData::Get().ecs;

	ECS::Pickup& pick_up = ecs->AddComponent(Pickup, entity);
	pick_up.onPickupFn = ApplyReboundRune;

	ECS::Collider& collider = ecs->AddComponent(Collider, entity);
	collider.SetFlag(ECS::Collider::PlayerOnly);
	collider.SetFlag(ECS::Collider::GhostCollider);
	collider.destroyOnContact = true;

	ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);
	transform.InitCollider(collider);

	return entity;
}

void CreateEntities(ECS::Entity& biome_entity)
{
	srand ((u32)time(NULL));

	std::unordered_map<BasicString, CreateEntityFn> CreateEntitiyFunctions;
	CreateEntitiyFunctions["PlayerSpawner"] = CreatePlayerSpawner;
	CreateEntitiyFunctions["Flower"] = CreateFlower;
	CreateEntitiyFunctions["Torch"] = CreateTorch;
	CreateEntitiyFunctions["Door"] = CreateDoor;
	CreateEntitiyFunctions["Pickup"] = CreatePickup;
	CreateEntitiyFunctions["BlindingSpider"] = BlindingSpider::Create;
	CreateEntitiyFunctions["ShockSweeper"] = ShockSweeper::Create;
	CreateEntitiyFunctions["ReboundRune"] = CreateRune;
	
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Biome& biome = ecs->GetComponentRef(Biome, biome_entity);
	for (u32 i = 0; i < biome.levels.size(); i++)
	{
		const ECS::Level& level = biome.levels[i];
		for (auto iter = level.entities.begin(); iter != level.entities.end(); iter++)
		{
			const char* entity_id = iter->first.c_str();

			if(CreateEntitiyFunctions.contains(entity_id))
			{
				CreateEntityFn create_fn = CreateEntitiyFunctions.at(entity_id);

				char buffer[64];
				snprintf(buffer, 64, "%sConfig", entity_id);

				ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>(buffer);

				const std::vector<ECS::Level::EntityMetaData>& entity_positions = iter->second;
				for( u32 e = 0; e < entity_positions.size(); e++ )
				{
					VectorF pos = entity_positions[e].position;
					if(!entity_positions[e].tag.empty())
						config->strings["tags"] = entity_positions[e].tag.c_str();

					create_fn(entity_id, buffer, pos);
				}
			}
			else
			{
				DebugPrint(Warning, "No CreateEntity function defined for %s", entity_id);
			}
		}
	}
}