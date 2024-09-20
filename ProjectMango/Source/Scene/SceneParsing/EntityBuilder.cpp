#include "pch.h"
#include "EntityBuilder.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "System/Files/ConfigManager.h"
#include "ECS/Components/Biome.h"
#include "ECS/Components/Animator.h"
#include "Core/Helpers.h"


typedef ECS::Entity (*CreateEntityFn)( const char* id, const char* config, VectorF spawn_pos );

static ECS::Entity CreateBasicObject(const char* id, const char* config_id, VectorF spawn_pos)
{
	// find the floor
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Entity entity = ecs->CreateEntity(id);

	const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>(config_id);

	// Transform
	ECS::Transform& transform = ecs->AddComponent(Transform, entity);
	transform.Init(config->values);
	transform.SetWorldPosition(spawn_pos - (transform.size / 2.0f));

	if(config->values.contains("snap_to_floor"))
	{
		if((bool)config->values.at("snap_to_floor"))
		{
			float distance = 0.0f;
			if( RaycastToFloor(entity, distance) )
			{
				transform.SetWorldPosition( transform.worldPosition + VectorF(0.0f, distance));
			}
		}
	}

	// Animation
	ECS::Animator& animator = ecs->AddComponent(Animator, entity);
	animator.Init(config->animation.c_str());
		
	if(config->values.contains("randomise_frame_start"))
	{
		if((bool)config->values.at("randomise_frame_start"))
		{
			int frame_start = (rand() % animator.GetActiveAnimation().frameCount) + 1;
			animator.frameIndex = frame_start;
		}
	}

	if(config->values.contains("randomise_frame_speed"))
	{
		float variation = config->values.at("randomise_frame_speed");

		int var_range = (int)(variation * 100.0f);

		int value = rand() % (int)(var_range * 2);
		float diff = (float)(value - var_range) / 100.0f;

		ECS::Animation& animation = animator.animations[animator.activeAnimation];
		animation.frameTime = animation.frameTime + (diff * animation.frameTime);
	}


	// Sprite
	ECS::Sprite& sprite = ecs->AddComponent(Sprite, entity);
	sprite.renderLayer = 6;

	return entity;
}

static ECS::Entity CreatePlayerSpawner(const char* id, const char* config_id, VectorF spawn_pos)
{
	ECS::Entity entity = CreateBasicObject(id, config_id, spawn_pos);

	// Spawner
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Spawner& spawner = ecs->AddComponent(Spawner, entity);

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


void CreateEntities(ECS::Biome& biome)
{
	srand (time(NULL));

	std::unordered_map<BasicString, CreateEntityFn> CreateEntitiyFunctions;
	CreateEntitiyFunctions["PlayerSpawner"] = CreatePlayerSpawner;
	CreateEntitiyFunctions["Flower"] = CreateFlower;
	CreateEntitiyFunctions["Torch"] = CreateTorch;

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