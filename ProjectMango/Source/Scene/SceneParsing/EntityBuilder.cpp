#include "pch.h"
#include "EntityBuilder.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "System/Files/ConfigManager.h"
#include "ECS/Components/Biome.h"
#include "ECS/Components/Animator.h"


static ECS::Entity CreateSpawner(const char* id, const char* config_id, const ECS::Level& level)
{
	// find the floor
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Entity entity = ecs->CreateEntity(id);

	const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>(config_id);

	if (!level.entities.contains(config->spawnId.c_str()))
		return ECS::EntityInvalid;

	// Transform
	ECS::Transform& transform = ecs->AddComponent(Transform, entity);
	transform.Init(config->values);

	VectorF spawn_pos = level.entities.at(config->spawnId.c_str());
	transform.SetWorldPosition(spawn_pos - (transform.size / 2.0f));

	// Animation
	ECS::Animator& animation = ecs->AddComponent(Animator, entity);
	animation.Init(config->animation.c_str());

	// Sprite
	ECS::Sprite& sprite = ecs->AddComponent(Sprite, entity);
	sprite.renderLayer = 8;
}


void CreateEntities(ECS::Biome& biome)
{
	for (u32 i = 0; i < biome.levels.size(); i++)
	{
		const ECS::Level& level = biome.levels[i];
		for (auto iter = level.entities.begin(); iter != level.entities.end(); iter++)
		{
			const char* entity_id = iter->first.c_str();

			if (StringCompare(entity_id, "PlayerSpawner"))
			{
				CreateSpawner("player_spawner", "PlayerSpawnerConfig", level);
			}
		}

	}
}