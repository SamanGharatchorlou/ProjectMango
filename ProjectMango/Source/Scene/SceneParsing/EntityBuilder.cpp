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
#include "Characters/Enemies/ShockSweeperEnemy.h"
#include "Characters/Enemies/BlindingSpiderEnemy.h"

typedef ECS::Entity (*CreateEntityFn)( const char* id, const char* config, VectorF spawn_pos );

static ECS::Entity CreateBasicObject(const char* id, const char* config_id, VectorF spawn_pos)
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
	VectorF pos = spawn_pos - (transform.size / 2.0f);
	transform.Init(config->values, pos);

	// Animation
	ECS::Animator& animator = ecs->GetComponentRef(Animator, entity);
	animator.Init(config->animation.c_str());
		
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

static ECS::Entity CreatePlayerSpawner(const char* id, const char* config_id, VectorF spawn_pos)
{
	ECS::Entity entity = CreateBasicObject(id, config_id, spawn_pos);

	// Spawner
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ecs->AddComponent(Spawner, entity);

	return entity;
}

static ECS::Entity CreateFlower(const char* id, const char* config_id, VectorF spawn_pos)
{
	return CreateBasicObject(id, config_id, spawn_pos);
}

static ECS::Entity CreateTorch(const char* id, const char* config_id, VectorF spawn_pos)
{
	return CreateBasicObject(id, config_id, spawn_pos);
}

static ECS::Entity CreateDoor(const char* id, const char* config_id, VectorF spawn_pos)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Entity entity = CreateBasicObject(id, config_id, spawn_pos);

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

	// Transform - sandwhich the door between the top and bottom raycast points
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


void CreateEntities(ECS::Entity& biome_entity)
{
	srand ((u32)time(NULL));

	std::unordered_map<BasicString, CreateEntityFn> CreateEntitiyFunctions;
	CreateEntitiyFunctions["PlayerSpawner"] = CreatePlayerSpawner;
	CreateEntitiyFunctions["Flower"] = CreateFlower;
	CreateEntitiyFunctions["Torch"] = CreateTorch;
	CreateEntitiyFunctions["Door"] = CreateDoor;
	CreateEntitiyFunctions["BlindingSpider"] = BlindingSpider::Create;
	CreateEntitiyFunctions["ShockSweeper"] = ShockSweeper::Create;
	
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

				const std::vector<VectorF>& entity_positions = iter->second;
				for( u32 e = 0; e < entity_positions.size(); e++ )
				{
					VectorF pos = entity_positions[e];
					create_fn(entity_id, buffer, pos);
				}
			}
		}
	}
}