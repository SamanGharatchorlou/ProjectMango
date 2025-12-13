#include "pch.h"
#include "EntityBuilder.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/GameComponents.h"
#include "ECS/Components/UIComponents.h"
#include "System/Files/ConfigManager.h"
#include "ECS/Components/Biome.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Collider.h"
#include "Graphics/Raycast.h"
#include "Entities/Enemies/ShockSweeperEnemy.h"
#include "Entities/Enemies/BlindingSpiderEnemy.h"
#include "UIEntityBuilder.h"


using namespace ECS;

// Base functions
Entity CreateBasicObject(const char* id, VectorF size)
{
	Entity entity = CreateEntity(id);

	// Transform
	Transform& transform = AddComponent(Transform, entity);
	transform.size = size;

	// Sprite
	Sprite& sprite = AddComponent(Sprite, entity);
	sprite.renderLayer = RenderLayer::BasicObject;
	sprite.canFlip = false;

	return entity;
}

Entity CreateBasicObject(const EntityMetaData& emd, bool has_config)
{
	Entity entity = CreateEntity(emd);

	const Config* config = nullptr;
	if(has_config)
		config = GetConfigFromEntity(entity);

	if (config)
	{
		// Transform
		Transform& transform = AddComponent(Transform, entity);
		transform.Init(config, emd);

		// Sprite
		Sprite& sprite = AddComponent(Sprite, entity);
		sprite.renderLayer = RenderLayer::BasicObject;
		sprite.canFlip = false;
		sprite.Init(config);

		if(sprite.texture && transform.size.isZero())
			DebugPrint(Warning, "CreateBasicObject - Have Sprite, but has no size");
	}
	else
	{
		// Transform
		Transform& transform = AddComponent(Transform, entity);
		transform.size = emd.size;
		transform.SetWorldPosition(emd.position - (emd.size * emd.pivotPoint));

		// Sprite
		Sprite& sprite = AddComponent(Sprite, entity);
		sprite.renderLayer = RenderLayer::BasicObject;
		sprite.canFlip = false;
		sprite.SetTexture(emd.spriteId.c_str());
		sprite.colourMod = emd.colourMod;
	}

	if(emd.isButton)
	{
		UIButton& button = AddComponent(UIButton, entity);
		button.UID = emd.uid;
	}

	return entity;
}

static Entity CreateAnimatedObject(const EntityMetaData& emd)
{
	Entity entity = CreateBasicObject(emd);
	const Config* config = GetConfigFromEntity(entity);

	// Animation
	Animator& animator = AddComponent(Animator, entity);
	animator.Init(config);

	return entity;
}

Entity CreateCoinStack(const EntityMetaData& emd)
{
	Entity entity = CreateBasicObject( emd, false );

	CoinStack& coin_stack = AddComponent(CoinStack, entity);
	coin_stack.capacity = 5;
	coin_stack.remaining = coin_stack.capacity;
	//coin_stack.colour = emd.colourMod;
	coin_stack.colourType = Colour::SColourToType(emd.colourMod);
	return entity;
}

void CreateEntities(Entity& biome_entity)
{
	srand ((u32)time(NULL));

	// game object entities
	std::unordered_map<BasicString, CreateEntityFn> CreateEntitiyFunctions;
	CreateEntitiyFunctions["Card"] = CreateCardEntity;
	CreateEntitiyFunctions["CoinStack"] = CreateCoinStack;
	CreateEntitiyFunctions["Text"] = CreateUIText;
	CreateEntitiyFunctions["Button"] = CreateUIButton;

	// UI entities
	CreateUIEntities();

	Biome& biome = GetComponentRef(Biome, biome_entity);
	for (u32 i = 0; i < biome.levels.size(); i++)
	{
		const Level& level = biome.levels[i];
		for (auto iter = level.entities.begin(); iter != level.entities.end(); iter++)
		{
			const char* type = iter->first.c_str();

			// create game object
			if(CreateEntitiyFunctions.contains(type))
			{
				CreateEntityFn create_fn = CreateEntitiyFunctions.at(type);

				const std::vector<EntityMetaData>& entitiy_meta_data = iter->second;
				for( u32 e = 0; e < entitiy_meta_data.size(); e++ )
				{
					create_fn(entitiy_meta_data[e]);
				}
			}
			else
			{
				DebugPrint(Warning, "No CreateEntity function defined for %s", type);
			}
		}
	}
}