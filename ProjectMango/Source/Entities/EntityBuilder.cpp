#include "pch.h"
#include "EntityBuilder.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/IncludeComponents.h"
#include "System/Files/ConfigManager.h"
#include "UIEntityBuilder.h"
#include "Entities/States/Behaviours.h"
#include "Game/States/GameState.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "Graphics/Raycast.h"
#include "Entities/ResourceBank.h"
#include "Game/Readers/AnimationReader.h"

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
	sprite.params.renderLayer = RenderLayer::BasicObject;

	return entity;
}

Entity CreateBasicObject(const EntityMetaData& emd)
{
	Entity entity = CreateEntity(emd);

	if(StringCompare(emd.GetID(), "Flower"))
		int a = 4;

	// Transform
	Transform& transform = AddComponent(Transform, entity);
	transform.Init(&emd);
	
	// Sprite
	bool has_sprite = emd.data.Contains("Sprite");
	bool is_sprite_sheet = emd.data.Contains("SpriteSheet");
	bool is_animator = emd.data.Contains("Animator");

	const Config* config = GetConfigFromEntity(entity);
	if(config || has_sprite || is_sprite_sheet || is_animator)
	{
		Sprite& sprite = AddComponent(Sprite, entity);
		if (config)
		{
			sprite.Init(config->data.GetString("Sprite"));
		}
		else if(has_sprite)
		{
			sprite.Init(emd.data.GetString("Sprite"));
		}		
		else if(is_sprite_sheet)
		{
			sprite.Init(emd.data.GetString("SpriteSheet"));
		}

		if(sprite.image.texture && transform.size.isZero())
			DebugPrint(Warning, "CreateBasicObject - Has Sprite, but has no size");
			
		sprite.params.renderLayer = RenderLayer::BasicObject;
		sprite.params.colourMod = emd.data.GetColour("Colour");

		// no sprite yet, try get a coloured version
		Colour::Type colour_type = (Colour::Type)emd.data.GetFloat("ColourType" , -1.0f);
		if(!sprite.image.texture && colour_type != -1)
		{
			StringBuffer64 coloured_sprite;
			if(has_sprite)
			{
				AddColourPostfix(emd.data.GetString("Sprite"), colour_type, coloured_sprite);
			}		
			else if(is_sprite_sheet)
			{
				AddColourPostfix(emd.data.GetString("SpriteSheet"), colour_type, coloured_sprite);
			}

			sprite.Init(coloured_sprite.c_str());
		}

		if(sprite.image.texture && is_sprite_sheet)
		{
			ASSERT( emd.data.GetVector("SpriteSheetFrames").lengthSquared() > 0, "Sprite sheet %d has frames counts == 0 (entity %s)", emd.data.GetString( "SpriteSheet" ), emd.GetID() );

			SpriteSheet& ss = AddComponent(SpriteSheet, entity);
			ss.Init( emd.data.GetVector("SpriteSheetFrames").toInt() );
		}

		if(is_animator)
		{
			Animator& animator = AddComponent(Animator, entity);
			animator.Init(emd);
			
			AddComponent(EntityState, entity);
		}
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
	coin_stack.colourType = (Colour::Type)emd.data.GetFloat("ColourType" , -1.0f);
	coin_stack.remaining = coin_stack.capacity;
	
	RegisterCoinResource(entity, Faction::GetTeam(emd.data.GetString("Faction")));

	return entity;
}

static void SetupCostIcons(Entity entity, int count)
{
	CoinStack& coin_stack = GetComponentRef(CoinStack, entity);

	DestroyChildren(entity);
	coin_stack.costEntities.resize(count);

	const Transform& transform = GetComponentRef(Transform, entity);
	VectorF size = transform.size;
	VectorF bottom = VectorF(size.x * 0.1f, size.y * 0.85f);

	for( u32 i = 0; i < count; i++ )
	{
		Entity child_entity = CreateEntity("coin_icon");
		coin_stack.costEntities[i] = (child_entity);

		EntityData::SetParent(child_entity, entity);

		// Transform
		VectorF size = VectorF(9,9);
		Transform& child_transform = AddComponent(Transform, child_entity);
		child_transform.size = size;

		VectorF offset =  VectorF(0.0f, i * 1.25f);
		VectorF local_position = bottom - (child_transform.size * offset);
		child_transform.SetLocalPosition( local_position );

		// Sprite
		Sprite& child_sprite = AddComponent(Sprite, child_entity);
		child_sprite.SetTexture("cost_filled");
		child_sprite.params.renderLayer = RenderLayer::UI;
		child_sprite.params.colourMod = Colour::s_typeToColour.at(coin_stack.colourType);
		child_sprite.params.colourMod.setOpacity(0.6f);
	}
}

static void SetupPowerIcons(Entity entity, int count)
{
	CoinStack& coin_stack = GetComponentRef(CoinStack, entity);

	DestroyChildren(entity);
	coin_stack.costEntities.resize(count);

	const Transform& transform = GetComponentRef(Transform, entity);
	VectorF size = transform.size;
	VectorF top = VectorF(size.x * 0.1f, size.y * 0.1f);

	for( u32 i = 0; i < count; i++ )
	{
		Entity child_entity = CreateEntity("coin_icon");
		coin_stack.costEntities[i] = (child_entity);

		EntityData::SetParent(child_entity, entity);

		// Transform
		VectorF size = VectorF(9,9);
		Transform& child_transform = AddComponent(Transform, child_entity);
		child_transform.size = size;

		VectorF offset =  VectorF(0.0f, i * 1.25f);
		VectorF local_position = top + (child_transform.size * offset);
		child_transform.SetLocalPosition( local_position );

		// Sprite
		Sprite& child_sprite = AddComponent(Sprite, child_entity);
		child_sprite.SetTexture("power_filled");
		child_sprite.params.renderLayer = RenderLayer::UI;
		child_sprite.params.colourMod = Colour::s_typeToColour.at(coin_stack.colourType);
		child_sprite.params.colourMod.setOpacity(0.6f);
	}
}

Entity CreateCoinPile(const EntityMetaData& emd)
{
	Entity entity = CreateBasicObject( emd );

	CoinStack& coin_stack = AddComponent(CoinStack, entity);
	coin_stack.isInventory = true;
	coin_stack.capacity = 5;
	coin_stack.colourType = (Colour::Type)emd.data.GetFloat("ColourType" , -1.0f);
	coin_stack.remaining = 0;

	SetupCostIcons(entity, coin_stack.capacity);

	RegisterCoinResource(entity, Faction::GetTeam(emd.data.GetString("Faction")));

	return entity;
}

Entity CreateCardPower(const EntityMetaData& emd)
{
	Entity entity = CreateBasicObject( emd );

	CoinStack& coin_stack = AddComponent(CoinStack, entity);
	coin_stack.isInventory = false;
	coin_stack.capacity = 5;
	coin_stack.colourType = (Colour::Type)emd.data.GetFloat("ColourType" , -1.0f);
	coin_stack.remaining = 0;

	SetupPowerIcons(entity, coin_stack.capacity);
	
	RegisterCardResource(entity, Faction::GetTeam(emd.data.GetString("Faction")));

	return entity;
}

Entity CreateHealthBar(const EntityMetaData& emd)
{
	Entity entity = CreateEntity(emd);
			
	// Transform
	Transform& transform = AddComponent(Transform, entity);
	transform.Init(&emd);

	LayeredSprite& layers = AddComponent(LayeredSprite, entity);
	layers.spriteLayers.push_back(LayeredSprite::Layer());
	layers.spriteLayers.push_back(LayeredSprite::Layer());

	Sprite& hb_bars = layers.spriteLayers[0].sprite;
	hb_bars.params.renderLayer = RenderLayer::BasicObject;
	hb_bars.SetTexture("HealthBarBars");

	Sprite& hb_health = layers.spriteLayers[1].sprite;
	hb_health.params.renderLayer = (RenderLayer)((int)RenderLayer::BasicObject - 1);
	hb_health.SetTexture("HealthBarHealth");

	return entity;
}

static ECS::Entity CreateSpawner(const ECS::EntityMetaData& emd)
{
	ECS::Entity entity = CreateBasicObject(emd);

	// Spawner
	AddComponent(Spawner, entity);

	Collider& collider = AddComponent(Collider, entity);
	collider.SetFlag(Collider::GhostCollider);
	collider.SetFlag(Collider::IgnoreTerrain);

	Transform& transform = GetComponentRef(Transform, entity);
	transform.Init(&emd, collider);

	return entity;
}

// something that moves
Entity CreateActor(const ECS::EntityMetaData& emd, const char* id_override)
{
	// adding everything something NEEDS to be an enemy... pretty much anyway
	Entity entity = ECS::CreateEntity(emd);
	if(id_override)
	{
		EntityData& ed = GetOrAddComponent(EntityData, entity); 
		ed.id = id_override;
	}

	// Collider
	Collider& collider = AddComponent(Collider, entity);

	// Transform
	Transform& transform = AddComponent(Transform, entity);
	transform.Init(&emd, collider);

	// MovementPhysics
	Physics& physics = AddComponent(Physics, entity);
	physics.Init();

	// Animator
	Animator& animation = AddComponent(Animator, entity);
	animation.Init();

	// Health
	Health& health = AddComponent(Health, entity);
	health.Init();
	
	// Sprite
	Sprite& sprite = AddComponent(Sprite, entity);
	sprite.Init(nullptr);
	sprite.params.renderLayer = RenderLayer::Characters;

	// EntityState
	EntityState& character_state = AddComponent(EntityState, entity);

	return entity;
}


// spawns from a card
Entity CreateMonster(const ECS::EntityMetaData& emd)
{
	Entity entity = CreateActor(emd, nullptr);

	AIController& ai = AddComponent(AIController, entity);
	AddComponent(AIIntent, entity);
	AddComponent(Pathing, entity);

	Collider& collider = GetComponentRef(Collider, entity);
	collider.Init();
	// remove this to prevent enemies passing each other
	collider.SetFlag(Collider::TerrainOnly);

	BehaviourState& state = AddComponent(BehaviourState, entity);
	state.Init();

	BehaviourMap& map = AddComponent(BehaviourMap, entity);
	PopulateMonsterBehaviours(map);

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
	meta_data.data.strings["Id"] = monster;
	
	// Transform
	Transform& transform = AddComponent(Transform, entity);
	transform.Init(nullptr);
	
	// Collider
	Collider& collider = AddComponent(Collider, entity);
	collider.Init();
	collider.SetFlag(Collider::IgnoreAll);
	
	Transform& parent_transform = GetComponentRef(Transform, parent);
	VectorF anchor = parent_transform.worldPosition + parent_transform.size * 0.65f;
	transform.SetObjectCenter(anchor);

	// Animator
	Animator& animation = AddComponent(Animator, entity);
	animation.Init();

	// Sprite
	Sprite& sprite = AddComponent(Sprite, entity);
	sprite.Init(nullptr);

	const Sprite& parent_sprite = GetComponentRef(Sprite, parent);
	sprite.params.renderLayer = (RenderLayer)((int)parent_sprite.params.renderLayer + 1);

	// EntityState
	AddComponent(EntityState, entity);

	return entity;
}

// Enemy
// ---------------------------------------------------------
// create an actual enemy i.e. the thing the player fights
Entity CreateEnemy(const ECS::EntityMetaData& emd)
{	
	// pick random enemy create enemy registry
	Entity entity = CreateActor(emd, "ShockSweeper");
	
	AddComponent(AIIntent, entity);
	AddComponent(Inventory, entity);

	Collider& collider = GetComponentRef(Collider, entity);
	collider.SetFlag(Collider::IsEnemy);

	AIController& ai = AddComponent(AIController, entity);
	ai.isDisabled = true;
	

	// Turn
	TurnState& turn = AddComponent(TurnState, entity);
	turn.initiative = 10;

	// mark ourselves as the enemy
	if(GameState* game_state = GameState::GetActive())
	{
		game_state->enemy = entity;
	}
		
	return entity;
}

// Enemy
// ---------------------------------------------------------
// create an actual enemy i.e. the thing the player fights
Entity CreatePlayer(const ECS::EntityMetaData& emd)
{	
	ECS::Entity entity = CreateActor(emd, nullptr);

	AddComponent(PlayerController, entity);
	AddComponent(AIIntent, entity);
	AddComponent(BehaviourState, entity);
	AddComponent(Inventory, entity);

	Collider& collider = GetComponentRef(Collider, entity);
	collider.SetFlag(Collider::IsPlayer);
	collider.SetFlag(Collider::Static);

	// Turn
	TurnState& turn = AddComponent(TurnState, entity);
	turn.initiative = 5;

	if(DebugMenu::GetSelectedEntity() == EntityInvalid)
		DebugMenu::SelectEntity(entity);
		
	return entity;
}

Entity CreateVFX(const char* vfx, const RectF& rect)
{
	if(!vfx || !AnimationReader::AnimationExists(vfx))
		return EntityInvalid;

	EntityMetaData data;
	data.data.strings["Id"] = vfx;
	data.data.vectors["Position"] = rect.TopLeft();
	data.data.vectors["Size"] = rect.Size();
	data.data.strings["Sprite"] = vfx;

	Entity entity = CreateBasicObject(data);

	// Animation
	Animator& animator = AddComponent(Animator, entity);
	animator.Init();
	animator.StartAnimation(Action::Active);

	DeathScentence& ds = AddComponent(DeathScentence, entity);
	ds.deathLoops = 1;
	ds.action = Action::Active;

	return entity;
}

static void PostProcess(Entity entity, const EntityMetaData& emd)
{
	if( emd.data.Contains("ButtonCallback") )
	{
		UIButton& button = GetOrAddComponent(UIButton, entity);
		button.callback = emd.data.GetString("ButtonCallback");
	}

	if( emd.data.Contains("Callback") )
	{
		Callback& cb = GetOrAddComponent(Callback, entity);
		cb.callback = emd.data.GetString("Callback");
	}

	if( emd.data.Contains("Faction") )
	{
		Faction& faction = GetOrAddComponent(Faction, entity);
		faction.team = Faction::GetTeam(emd.data.GetString("Faction"));
	}
	
	if( emd.data.GetBool("SnapToFloor") )
	{
		float distance = 0.0f;
		if( RaycastToFloor(entity, distance) )
		{
			Transform& transform = GetComponentRef(Transform, entity);
			transform.SetWorldPosition( transform.worldPosition + VectorF(0.0f, distance));
		}
	}

	ASSERT(GetComponentRef(Transform, entity).size.isPositive(), "%s: Invalid Transform, has size 0", GetName(entity));
}

void CreateEntities(Entity& biome_entity)
{
	srand ((u32)time(NULL));

	// game object entities
	std::unordered_map<BasicString, CreateEntityFn> CreateEntitiyFunctions;
	CreateEntitiyFunctions["Card"] = CreateCardEntity;
	CreateEntitiyFunctions["CoinStack"] = CreateCoinStack;
	CreateEntitiyFunctions["CoinPile"] = CreateCoinPile;
	CreateEntitiyFunctions["CardPower"] = CreateCardPower;
	CreateEntitiyFunctions["Text"] = CreateUIText;
	CreateEntitiyFunctions["HealthBar"] = CreateHealthBar;
	CreateEntitiyFunctions["Enemy"] = CreateEnemy;
	CreateEntitiyFunctions["Spawner"] = CreateSpawner;
	CreateEntitiyFunctions["Player"] = CreatePlayer;

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
			CreateEntityFn create_fn = CreateBasicObject;
			if(CreateEntitiyFunctions.contains(type))
			{
				create_fn = CreateEntitiyFunctions.at(type);
			}

			const std::vector<EntityMetaData>& entitiy_meta_data = iter->second;
			for( u32 e = 0; e < entitiy_meta_data.size(); e++ )
			{
				Entity entity = create_fn(entitiy_meta_data[e]);
				PostProcess(entity, entitiy_meta_data[e]);
			}
		}
	}
}