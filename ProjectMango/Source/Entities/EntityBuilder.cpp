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
#include "ECS/Components/Physics.h"
#include "Graphics/Raycast.h"
#include "Entities/Enemies/ShockSweeperEnemy.h"
#include "Entities/Enemies/BlindingSpiderEnemy.h"
#include "UIEntityBuilder.h"
#include "ECS/Components/AIComponents.h"
#include "Entities/States/Behaviours.h"

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

	const Config* config = GetConfigFromEntity(entity);

	// change this, does need this if here should be the same with the correct fallbacks
	if (config)
	{
		// Transform
		Transform& transform = AddComponent(Transform, entity);
		transform.Init(&emd);

		// Sprite
		Sprite& sprite = AddComponent(Sprite, entity);
		sprite.renderLayer = RenderLayer::BasicObject;
		sprite.canFlip = false;
		sprite.Init();
		sprite.colourMod = emd.colourMod;

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
		sprite.ID = emd.spriteId.c_str();
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

	if(!emd.callback.empty())
	{
		Callback& cb = AddComponent(Callback, entity);
		cb.callback = emd.callback;
	}

	return entity;
}

static Entity CreateAnimatedObject(const EntityMetaData& emd)
{
	Entity entity = CreateBasicObject(emd);

	// Animation
	Animator& animator = AddComponent(Animator, entity);
	animator.Init();

	// EntityState
	EntityState& character_state = AddComponent(EntityState, entity);

	return entity;
}

Entity CreateCoinStack(const EntityMetaData& emd)
{
	Entity entity = CreateBasicObject( emd );

	CoinStack& coin_stack = AddComponent(CoinStack, entity);
	coin_stack.isInventory = false;
	coin_stack.capacity = 5;
	coin_stack.colourType = Colour::Type(emd.colourType);
	coin_stack.spritePrefix = emd.spriteId;
	coin_stack.remaining = coin_stack.capacity;

	return entity;
}

Entity CreateCoinPile(const EntityMetaData& emd)
{
	Entity entity = CreateBasicObject( emd );

	SpriteCycle& cycle = AddComponent(SpriteCycle, entity);
	cycle.spritePrefix = emd.spriteId;
	cycle.index = 0;

	CoinStack& coin_stack = AddComponent(CoinStack, entity);
	coin_stack.isInventory = true;
	coin_stack.capacity = 100;
	coin_stack.colourType = Colour::Type(emd.colourType);
	coin_stack.spritePrefix = emd.spriteId;
	coin_stack.remaining = 0;

	return entity;
}

Entity CreateHealthBar(const EntityMetaData& emd)
{
	Entity entity = CreateEntity(emd);
			
	// Transform
	Transform& transform = AddComponent(Transform, entity);
	transform.size = emd.size;
	transform.SetWorldPosition(emd.position - (emd.size * emd.pivotPoint));

	LayeredSprite& layers = AddComponent(LayeredSprite, entity);
	layers.spriteLayers.push_back(LayeredSprite::Layer());
	layers.spriteLayers.push_back(LayeredSprite::Layer());

	Sprite& hb_bars = layers.spriteLayers[0].sprite;
	hb_bars.renderLayer = RenderLayer::BasicObject;
	hb_bars.SetTexture("HealthBarBars");

	Sprite& hb_health = layers.spriteLayers[1].sprite;
	hb_health.renderLayer = (RenderLayer)((int)RenderLayer::BasicObject - 1);
	hb_health.SetTexture("HealthBarHealth");

	if(!emd.callback.empty())
	{
		Callback& cb = AddComponent(Callback, entity);
		cb.callback = emd.callback;
	}

	return entity;
}

static ECS::Entity CreateSpawner(const ECS::EntityMetaData& emd)
{
	ECS::Entity entity = CreateAnimatedObject(emd);

	// Spawner
	AddComponent(Spawner, entity);

	return entity;
}

// spawns from a card
Entity CreateMonster(const ECS::EntityMetaData& emd)
{
	Entity entity = CreateActor(emd);

	AddComponent(AIController, entity);
	AddComponent(AIIntent, entity);
	AddComponent(Pathing, entity);

	BehaviourState& state = AddComponent(BehaviourState, entity);
	state.Init();

	BehaviourMap& map = AddComponent(BehaviourMap, entity);
	std::vector<Action::Enum> states;
	states.push_back(Action::Idle);
	states.push_back(Action::Run);
	states.push_back(Action::BasicAttack);
	states.push_back(Action::Death);

	PopulateMonsterBehaviours(map, states);

	Target& target = AddComponent(Target, entity);
	target.target = Target::GetEnemy();

	return entity;
}

// something that moves
Entity CreateActor(const ECS::EntityMetaData& emd)
{
	// adding everything something NEEDS to be an enemy... pretty much anyway
	Entity entity = ECS::CreateEntity(emd);

	// Collider
	Collider& collider = AddComponent(Collider, entity);

	// Transform
	Transform& transform = AddComponent(Transform, entity);
	transform.Init(&emd, collider);

	// TODO:: I need to set the collider relative rect here, i need it for raycasts etc
	// and really it shouldnt be setup by the animator, thats weird, it should be a fixed value
	// and setup and this point and then never change
	collider->SetRelativeRect(animation.entityColliderPos, animation.entityColliderSize);

	// MovementPhysics
	Physics& physics = AddComponent(Physics, entity);
	physics.Init();

	// Animator
	Animator& animation = AddComponent(Animator, entity);
	animation.Init();

	// Health
	Health& health = AddComponent(Health, entity);
	health.Init();

	// set sprite layer - default 5
	Sprite& sprite = AddComponent(Sprite, entity);
	sprite.Init();
	sprite.renderLayer = RenderLayer::Characters;

	// EntityState
	EntityState& character_state = AddComponent(EntityState, entity);

	return entity;
}

// the thing on the card, mostly just an animated object
Entity CreateCardActor(const char* monster, Entity parent)
{
	// adding everything something NEEDS to be an enemy... pretty much anyway
	Entity entity = ECS::CreateEntity(monster);
		
	// set card as parent
	EntityData::SetParent(entity, parent);

	EntityMetaData meta_data;
	meta_data.id = monster;

	// Transform
	Transform& transform = AddComponent(Transform, entity);
	transform.Init(nullptr);
	
	Transform& parent_transform = GetComponentRef(Transform, parent);
	VectorF child_position = parent_transform.size * 0.5f;// - spider_transform.size * 0.5f;
	transform.SetLocalPosition( child_position );

	// Animator
	Animator& animation = AddComponent(Animator, entity);
	animation.Init();

	// set sprite layer - default 5
	Sprite& sprite = AddComponent(Sprite, entity);
	sprite.Init();

	const Sprite& parent_sprite = GetComponentRef(Sprite, parent);
	sprite.renderLayer = (RenderLayer)((int)parent_sprite.renderLayer + 1);

	// EntityState
	EntityState& character_state = AddComponent(EntityState, entity);

	return entity;
}

void CreateEntities(Entity& biome_entity)
{
	srand ((u32)time(NULL));

	// game object entities
	std::unordered_map<BasicString, CreateEntityFn> CreateEntitiyFunctions;
	CreateEntitiyFunctions["Card"] = CreateCardEntity;
	CreateEntitiyFunctions["CoinStack"] = CreateCoinStack;
	CreateEntitiyFunctions["CoinPile"] = CreateCoinPile;
	CreateEntitiyFunctions["Text"] = CreateUIText;
	CreateEntitiyFunctions["Button"] = CreateBasicObject;
	CreateEntitiyFunctions["Sprite"] = CreateBasicObject;
	CreateEntitiyFunctions["HealthBar"] = CreateHealthBar;
	//CreateEntitiyFunctions["TrainingDummy"] = TrainingDummy::Create;
	CreateEntitiyFunctions["ShockSweeper"] = ShockSweeper::CreateEnemy;
	//CreateEntitiyFunctions["BlindingSpider"] = BlindingSpider::Create;
	CreateEntitiyFunctions["PlayerSpawner"] = CreateSpawner;

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