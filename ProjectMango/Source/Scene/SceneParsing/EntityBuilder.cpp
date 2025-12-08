#include "pch.h"
#include "EntityBuilder.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
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

Entity CreateBasicObject(const EntityMetaData& emd)
{
	Entity entity = CreateEntity(emd);
	if (const Config* config = GetConfig(entity))
	{
		// Transform
		Transform& transform = AddComponent(Transform, entity);
		transform.Init(config, emd.position);

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
		transform.SetWorldPosition(emd.position - (emd.size * 0.5f));

		// Sprite
		Sprite& sprite = AddComponent(Sprite, entity);
		sprite.renderLayer = RenderLayer::BasicObject;
		sprite.canFlip = false;
		sprite.SetTexture(emd.spriteId.c_str());
		sprite.colourMod = emd.colourMod;
	}

	if(emd.isButton)
	{
		AddComponent(UIButton, entity);
	}

	return entity;
}

static Entity CreateAnimatedObject(const EntityMetaData& emd)
{
	Entity entity = CreateBasicObject(emd);
	const Config* config = GetConfig(entity);

	// Animation
	Animator& animator = AddComponent(Animator, entity);
	animator.Init(config);

	return entity;
}

// Entitiy creation callbacks
static Entity CreatePlayerSpawner(const EntityMetaData& emd)
{
	Entity entity = CreateAnimatedObject(emd);

	// Spawner
	AddComponent(Spawner, entity);

	return entity;
}

static Entity CreateFlower(const EntityMetaData& emd)
{
	return CreateAnimatedObject(emd);
}

static Entity CreateTorch(const EntityMetaData& emd)
{
	return CreateAnimatedObject(emd);
}

static Entity CreateDoor(const EntityMetaData& emd)
{
	Entity entity = CreateAnimatedObject(emd);

	AddComponent(Door, entity);

	const Level& level = Biome::GetLevel(entity);	
	std::vector<u32> collider_flags;
	collider_flags.push_back(Collider::IsTerrain);

	RaycastResult down_result;
	Raycast(emd.position, VectorF(0.0, 1.0f), level.size.y, down_result, nullptr, &collider_flags);

	RaycastResult up_result;
	Raycast(emd.position, VectorF(0.0, -1.0f), level.size.y, up_result, nullptr, &collider_flags);

	// no valid door position
	if(!down_result.hasHit || !up_result.hasHit)
	{
		ecs->entities.KillEntity(entity);
		return EntityInvalid;
	}

	// Door
	Door& door = GetComponentRef(Door, entity);
	door.Init();

	const Config* config = ConfigManager::Get()->GetConfig(emd.id.c_str());
	door.triggerRange = config->data.GetFloat("trigger_range");

	// Transform - sandwich the door between the top and bottom raycast points
	Transform& transform = GetComponentRef(Transform, entity);

	const VectorF top = up_result.hitPosition;
	const VectorF bot = down_result.hitPosition;
	const float height = bot.y - top.y;
	const float size_ratio = height /  transform.size.y;

	// update the transform positions
	transform.size = transform.size * size_ratio;
	transform.SetWorldPosition(top);

	door.GenerateColliders(config->data.GetFloat("collider_width"));
	
	return entity;
}

Entity CreateRune(const EntityMetaData& emd)
{
	Entity entity = CreateBasicObject( emd );

	Pickup& pick_up = AddComponent(Pickup, entity);
	pick_up.itemId = emd.id;
	//pick_up.config = emd.id;

	Collider& collider = AddComponent(Collider, entity);
	collider.SetFlag(Collider::PlayerOnly);
	collider.SetFlag(Collider::GhostCollider);
	collider.destroyOnContact = true;

	Transform& transform = GetComponentRef(Transform, entity);
	transform.InitCollider(collider);

	return entity;
}

Entity CreateCard(const EntityMetaData& emd)
{
	Entity entity = CreateBasicObject( emd );
	return entity;
}

Entity CreateCoinStack(const EntityMetaData& emd)
{
	Entity entity = CreateBasicObject( emd );

	CoinStack& coin_stack = AddComponent(CoinStack, entity);
	coin_stack.capacity = 5;
	coin_stack.remaining = coin_stack.capacity;
	coin_stack.colour = emd.colourMod;

	// convert SColour into CoinStack colour
	SColour::Enum colour_type = emd.colourMod.GetColosestColour();
	CoinStack::ColourType type = CoinStack::ColourType::Count;
	switch( colour_type )
	{
		case SColour::White:
		coin_stack.colourType = CoinStack::ColourType::White;
		break;

		case SColour::Blue:
		coin_stack.colourType = CoinStack::ColourType::Blue;
		break;

		case SColour::Black:
		coin_stack.colourType = CoinStack::ColourType::Black;
		break;	

		case SColour::Red:
		coin_stack.colourType = CoinStack::ColourType::Red;
		break;
		
		case SColour::Green:
		coin_stack.colourType = CoinStack::ColourType::Green;
		break;

		case SColour::Count:	
		case SColour::None:
		case SColour::Purple:
		case SColour::Yellow:
		case SColour::LightGrey:
		case SColour::MidGrey:
		default:		
		break;
	}

	return entity;
}

Entity CreateText(const EntityMetaData& emd)
{
	Entity entity = CreateEntity(emd.id.c_str());

	// Transform
	Transform& transform = AddComponent(Transform, entity);
	transform.size = emd.size;
	transform.SetWorldPosition(emd.position);// - (emd.size * 0.5f));
	
	// UIText
	UIText& ui_text = AddComponent(UIText, entity);
	ui_text.UID = emd.uid;
	ui_text.font.Resize(50);

	return entity;
}

void CreateEntities(Entity& biome_entity)
{
	srand ((u32)time(NULL));

	// game object entities
	std::unordered_map<BasicString, CreateEntityFn> CreateEntitiyFunctions;
	CreateEntitiyFunctions["PlayerSpawner"] = CreatePlayerSpawner;
	CreateEntitiyFunctions["Flower"] = CreateFlower;
	CreateEntitiyFunctions["Torch"] = CreateTorch;
	CreateEntitiyFunctions["Door"] = CreateDoor;
	CreateEntitiyFunctions["BlindingSpider"] = BlindingSpider::Create;
	CreateEntitiyFunctions["ShockSweeper"] = ShockSweeper::Create;
	CreateEntitiyFunctions["TrainingDummy"] = TrainingDummy::Create;
	CreateEntitiyFunctions["Rune"] = CreateRune;
	CreateEntitiyFunctions["Card"] = CreateCard;
	CreateEntitiyFunctions["CoinStack"] = CreateCoinStack;
	CreateEntitiyFunctions["Text"] = CreateText;

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
			//else if(IsUIEntity(type))
			//{
			//	const std::vector<EntityMetaData>& entitiy_meta_data = iter->second;
			//	for( u32 e = 0; e < entitiy_meta_data.size(); e++ )
			//	{
			//		CreateUIEntity(type, entitiy_meta_data[e]);
			//	}
			//}
			else
			{
				DebugPrint(Warning, "No CreateEntity function defined for %s", type);
			}
		}
	}
}