#include "pch.h"
#include "EntityBuilder.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/IncludeComponents.h"
#include "System/Files/ConfigManager.h"
#include "UIEntityBuilder.h"
#include "Entities/States/Behaviours.h"
#include "Game/States/GameState.h"
#include "Debugging/ImGui/ImGuiMenu.h"
#include "Graphics/Raycast.h"
#include "Entities/Registries/ResourceBank.h"
#include "Entities/Registries/RelicRegistry.h"
#include "Entities/Registries/CardRegistry.h"
#include "Game/Readers/AnimationReader.h"
#include "Core/Helpers.h"
#include "Entities/Factory/ComponentAssembler.h"

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
	if (emd.data.Contains("id"))
	{
		if (const Config* config = GetConfig(emd.data.GetString("id")))
		{
			EntityMetaData meta_data;
			meta_data.data.Merge(emd.data);
			meta_data.data.Merge(config->data);
			
			// merge config data into meta data, config overrides default meta data
			return AssembleEntity(emd);
		}
	}

	return AssembleEntity(emd);
}

Entity CreateCoinStack(const EntityMetaData& emd)
{
	Entity entity = CreateBasicObject( emd );

	CoinStack& coin_stack = AddComponent(CoinStack, entity);
	coin_stack.isInventory = false;
	coin_stack.capacity = 5;
	coin_stack.colourType = (Colour::Type)emd.data.GetFloat("colour_type" , -1.0f);
	coin_stack.remaining = coin_stack.capacity;
	
	RegisterCoinResource(entity, Faction::GetTeam(emd.data.GetString("faction")));

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
		size = AdjustToScreenSize(size);

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

static Entity CreateCoinPile(const EntityMetaData& emd)
{
	Entity entity = CreateBasicObject( emd );

	CoinStack& coin_stack = AddComponent(CoinStack, entity);
	coin_stack.isInventory = true;
	coin_stack.capacity = 5;
	coin_stack.colourType = (Colour::Type)emd.data.GetFloat("colour_type" , -1.0f);
	coin_stack.remaining = 0;

	SetupCostIcons(entity, coin_stack.capacity);

	RegisterCoinResource(entity, Faction::GetTeam(emd.data.GetString("faction")));

	return entity;
}

static Entity CreateCardPower(const EntityMetaData& emd)
{
	Entity entity = CreateBasicObject( emd );

	CoinStack& coin_stack = AddComponent(CoinStack, entity);
	coin_stack.isInventory = false;
	coin_stack.capacity = 5;
	coin_stack.colourType = (Colour::Type)emd.data.GetFloat("colour_type" , -1.0f);
	coin_stack.remaining = 0;

	SetupPowerIcons(entity, coin_stack.capacity);
	
	RegisterCardResource(entity, Faction::GetTeam(emd.data.GetString("faction")));

	return entity;
}

static Entity CreateHealthBar(const EntityMetaData& emd)
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

// something that moves
Entity CreateActor(const EntityMetaData& emd, const char* id_override)
{
	// adding everything something NEEDS to be an enemy... pretty much anyway
	Entity entity = CreateEntity(emd);
	if(id_override)
	{
		EntityData& ed = GetOrAddComponent(EntityData, entity); 
		ed.id = id_override;
	}

	if (const Config* config = GetConfigFromEntity(entity))
	{
		EntityMetaData meta_data;
		meta_data.data.Merge(emd.data);
		meta_data.data.Merge(config->data);

		// merge config data into meta data, config overrides default meta data
		return AssembleEntity(meta_data);
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

	if (emd.data.Contains("faction"))
	{
		Faction& faction = GetOrAddComponent(Faction, entity);
		faction.team = Faction::GetTeam(emd.data.GetString("faction"));
	}

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
Entity CreateCardSpell(const EntityMetaData& emd, Entity parent)
{
	// adding everything something NEEDS to be an enemy... pretty much anyway
	Entity entity = ECS::CreateEntity(emd);

	// set card as parent
	EntityData::SetParent(entity, parent);

	// Transform
	Transform& transform = AddComponent(Transform, entity);
	transform.Init(&emd);

	Transform& parent_transform = GetComponentRef(Transform, parent);
	VectorF anchor = parent_transform.worldPosition + parent_transform.size * 0.65f;
	transform.SetObjectCenter(parent_transform.GetObjectCenter());

	RectF rect = transform.GetRect();
	Resize(rect, parent_transform.GetRect());
	transform.size = rect.Size();
	transform.SetObjectCenter(parent_transform.GetObjectCenter());

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
	const char* enemy_type = emd.data.GetString("enemy_type");
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

	strategy.strategies.push_back(approach_pattern);
	strategy.strategies.push_back(attack_pattern);
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
		//inventory.relics.push_back(*relic);
	}
	if(ECS::Relic* relic = RelicRegistry::GetRelic("IncreaseColourDrawRate"))
	{
		//inventory.relics.push_back(*relic);
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

Entity CreateVFX(const char* vfx, const RectF& rect)
{
	if(!vfx || !AnimationReader::AnimationExists(vfx))
		return EntityInvalid;

	EntityMetaData meta_data;
	PopulateMetaData(vfx, rect, meta_data);

	Entity entity = CreateBasicObject(meta_data);

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
	// todo: remove these
	if( emd.data.Contains("button_callback") )
	{
		UIButton& button = GetOrAddComponent(UIButton, entity);
		button.callback = emd.data.GetString("button_callback");
	}

	if( emd.data.Contains("callback") )
	{
		Callback& cb = GetOrAddComponent(Callback, entity);
		cb.callback = emd.data.GetString("callback");
	}

	if( emd.data.Contains("faction") )
	{
		Faction& faction = GetOrAddComponent(Faction, entity);
		faction.team = Faction::GetTeam(emd.data.GetString("faction"));
	}
	
	if( emd.data.GetBool("snap_to_floor") )
	{
		RaycastResult result;
		if( RaycastToFloor(entity, result) )
		{
			Transform& transform = GetComponentRef(Transform, entity);
			transform.SetWorldPosition( transform.worldPosition + VectorF(0.0f, result.distance));
		}
	}
	if( emd.data.GetBool("snap_to_wall_right") )
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

static std::unordered_map<BasicString, CreateEntityFn> s_createEntitiyFunctions;

static void InitEntityFunctions()
{
	// game object entities
	//s_createEntitiyFunctions["Card"] = CreateCardEntity;
	s_createEntitiyFunctions["CoinStack"] = CreateCoinStack;
	s_createEntitiyFunctions["CoinPile"] = CreateCoinPile;
	s_createEntitiyFunctions["CardPower"] = CreateCardPower;
	//s_createEntitiyFunctions["Text"] = CreateUIText;
	s_createEntitiyFunctions["HealthBar"] = CreateHealthBar;
	s_createEntitiyFunctions["Enemy"] = CreateEnemy;
	//s_createEntitiyFunctions["Spawner"] = CreateSpawner;
	s_createEntitiyFunctions["Player"] = CreatePlayer;
}


Entity CreateEntityFromData(const EntityMetaData& meta_data)
{
	if (s_createEntitiyFunctions.size() == 0)
		InitEntityFunctions();

	const char* type = meta_data.GetID();

	// create game object`
	CreateEntityFn create_fn = CreateBasicObject;
	if (s_createEntitiyFunctions.contains(type))
	{
		create_fn = s_createEntitiyFunctions.at(type);
	}

	Entity entity = create_fn(meta_data);
	PostProcess(entity, meta_data);

	return entity;
}

void CreateEntities(Entity& biome_entity)
{
	if (s_createEntitiyFunctions.size() == 0)
		InitEntityFunctions();

	// UI entities
	CreateUIEntities();

	Biome& biome = GetComponentRef(Biome, biome_entity);
	for (u32 i = 0; i < biome.levels.size(); i++)
	{
		const Level& level = biome.levels[i];

		for (u32 i = 0; i < level.entityMetaData.size(); i++)
		{
			const EntityMetaData& emd = level.entityMetaData[i];
			Entity entity = CreateEntityFromData(emd);
		}
	}
}
