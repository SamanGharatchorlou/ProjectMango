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
#include "Entities/Registries/ResourceBank.h"
#include "Entities/Registries/RelicRegistry.h"
#include "Entities/Registries/CardRegistry.h"
#include "Game/Readers/AnimationReader.h"
#include "Core/Helpers.h"

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
		VectorF size = VectorF(15,15);
		size = AdjustToScreenSize(size);
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
		VectorF size = VectorF(15,15);
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
	EntityState& state = AddComponent(EntityState, entity);
	state.current = Action::Inactive;

	Collider& collider = AddComponent(Collider, entity);
	collider.SetFlag(Collider::GhostCollider);
	collider.SetFlag(Collider::IgnoreTerrain);

	Transform& transform = GetComponentRef(Transform, entity);
	transform.Init(&emd, collider);

	Animator& animator = GetComponentRef(Animator, entity);
	animator.state = TimeState::Stopped;

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
	sprite.params.renderLayer = RenderLayer::Monsters;

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

	// Turn
	TurnState& turn = AddComponent(TurnState, entity);
	// insert ourself after the player, before the enemy once we've been summond
	turn.initiative = 8;

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

static void BuildIntentIconEntity(Entity icon_entity)
{
	UIIntentIcon& icon = AddComponent(UIIntentIcon, icon_entity);
	icon.displays.push_back({ "basic_approach_icon"	, EnemyPhase::Approach });
	icon.displays.push_back({ "basic_attack_icon"	, EnemyPhase::Attack });
	icon.displays.push_back({ "basic_recovery_icon"	, EnemyPhase::Recover });
	icon.displays.push_back({ "debuff_icon"			, EnemyPhase::Debuff });

	// Transform
	Transform& child_transform = AddComponent(Transform, icon_entity);
	VectorF size = VectorF(16,16);
	child_transform.size = AdjustToScreenSize(size);

	
	Entity parent = GetParent(icon_entity);
	const Transform& parent_transform = GetComponentRef(Transform, parent);
	VectorF object_tc = parent_transform.GetObjectRect().TopCenter();
	VectorF top_left = parent_transform.GetRect().TopLeft();


	VectorF local_position = VectorF(object_tc.x- top_left.x, (object_tc.y - top_left.y) * 0.25f );
	child_transform.SetLocalPosition( local_position );

	// Sprite
	Sprite& child_sprite = AddComponent(Sprite, icon_entity);
	child_sprite.params.renderLayer = RenderLayer::UI;
	child_sprite.params.colourMod.setOpacity(0.85f);
	//child_sprite.SetTexture( "basic_recovery_icon" );
}

// Enemy
// ---------------------------------------------------------
// create an actual enemy i.e. the thing the player fights
Entity CreateEnemy(const ECS::EntityMetaData& emd)
{	
	// pick random enemy create enemy registry
	const char* enemy_type = emd.data.GetString("EnemyType");
	Entity entity = CreateActor(emd, enemy_type);
	
	AddComponent(AIIntent, entity);
	AddComponent(Inventory, entity);
	
	// AIStrategy
	AIStrategy& strategy = AddComponent(AIStrategy, entity);
	EnemyPhase recover { EnemyPhase::Recover, 1 };
	EnemyPhase approach { EnemyPhase::Approach, 99 }; // once we reach the target
	EnemyPhase attack { EnemyPhase::Attack, 1 };
	EnemyPhase debuff { EnemyPhase::Debuff, 1 };
	
	Strategy approach_pattern;
	approach_pattern.name = "Approach";
	approach_pattern.phases.push_back(recover);
	approach_pattern.phases.push_back(approach);

	Strategy attack_pattern; 
	attack_pattern.name = "Attack";
	attack_pattern.phases.push_back(recover);
	attack_pattern.phases.push_back(attack);
	attack_pattern.phases.push_back(recover);

	Strategy debuff_pattern; 
	debuff_pattern.name = "Debuff";
	debuff_pattern.phases.push_back(recover);
	debuff_pattern.phases.push_back(debuff);
	debuff_pattern.phases.push_back(recover);

	//strategy.strategies.push_back(approach_pattern);
	//strategy.strategies.push_back(attack_pattern);
	strategy.strategies.push_back(debuff_pattern);

	strategy.currentPhase = 0;
	strategy.turnsLeft = 1;
	
	// UIIntentIcon
	Entity child_entity = CreateEntity("intent_icon");
	EntityData::SetParent(child_entity, entity);
	BuildIntentIconEntity(child_entity);

	// Collider
	Collider& collider = GetComponentRef(Collider, entity);
	collider.SetFlag(Collider::IsEnemy);

	// AIController
	AIController& ai = AddComponent(AIController, entity);
	ai.isDisabled = false;
	
	// BehaviourState
	BehaviourState& state = AddComponent(BehaviourState, entity);
	state.Init();

	BehaviourMap& map = AddComponent(BehaviourMap, entity);
	PopulateDefaultBehaviours(map);

	// link this up the to kind of attack or phase
	//Damage& damage = AddComponent(Damage, entity);
	//damage.value = 1;

	// Turn
	TurnState& turn = AddComponent(TurnState, entity);
	turn.initiative = 10;

	// mark ourselves as the enemy
	if(GameState* game_state = GameState::GetActive())
	{
		game_state->enemy = entity;
	}
		
	if(DebugMenu::GetSelectedEntity() == EntityInvalid)
		DebugMenu::SelectEntity(entity);

	return entity;
}

// Player
// ---------------------------------------------------------
Entity CreatePlayer(const ECS::EntityMetaData& emd)
{	
	ECS::Entity entity = CreateActor(emd, nullptr);

	// todo(saman): does the player need all this stuff?
	AddComponent(PlayerController, entity);
	//AddComponent(AIIntent, entity);
	AddComponent(BehaviourState, entity);

	Inventory& inventory = AddComponent(Inventory, entity);
	if(ECS::Relic* relic = RelicRegistry::GetRelic("DiscountCardCost"))
	{
		inventory.relics.push_back(*relic);
	}
	if(ECS::Relic* relic = RelicRegistry::GetRelic("IncreaseDrawRate"))
	{
		inventory.relics.push_back(*relic);
	}

	Collider& collider = GetComponentRef(Collider, entity);
	collider.SetFlag(Collider::IsPlayer);
	collider.SetFlag(Collider::Static);

	// Turn
	TurnState& turn = AddComponent(TurnState, entity);
	turn.initiative = 5;

	// edit the render layer from the default (Monsters)
	Sprite& sprite = GetComponentRef(Sprite, entity);
	sprite.params.renderLayer = RenderLayer::Player;

	//if(DebugMenu::GetSelectedEntity() == EntityInvalid)
	//	DebugMenu::SelectEntity(entity);
		
	return entity;
}

// Card
// ---------------------------------------------------------
Entity CreateCardEntity(const EntityMetaData& emd)
{
	Entity entity = CreateBasicObject( emd );
	Card& card = AddComponent(Card, entity);
	card.tier = emd.data.GetInt("Tier");

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
		RaycastResult result;
		if( RaycastToFloor(entity, result) )
		{
			Transform& transform = GetComponentRef(Transform, entity);
			transform.SetWorldPosition( transform.worldPosition + VectorF(0.0f, result.distance));
		}
	}
	if( emd.data.GetBool("SnapToWall_Right") )
	{
		VectorF direction(1.0f,0.0f);
		RaycastResult result;
		if( RaycastToWall(entity, direction, result) )
		{
			Transform& transform = GetComponentRef(Transform, entity);
			VectorF position = transform.worldPosition + VectorF(result.distance, 0.0f);

			// they will be hugging the wall, so leave a gap
			if(result.distance > 0.0f)
				position = position - VectorF(50.0f, 0.0f);

			transform.SetWorldPosition( position );
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

void DrawCards()
{
	ComponentArray<Card>& cards =  GetAllComponents(Card);
	
	// [ entity, tier ] 
	// push these into a list first, otherwise we can invalidat the iterator
	std::vector< std::pair<Entity,int> > entities;
	for( auto iter = cards.entityToComponent.begin(); iter != cards.entityToComponent.end(); iter++ )
	{
		const Card& card = cards.GetComponentByIndex(iter->second);
		entities.push_back({iter->first, card.tier});
	}

	for( u32 i = 0; i < entities.size(); i++ )
	{
		CardRegistry::DrawRandomCard(entities[i].first, entities[i].second);
	}
}