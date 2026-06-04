#include "pch.h"
#include "EntityBuilder.h"

#include "Debugging/ImGui/ImGuiMenu.h"
#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "EnemyBuilder.h"
#include "Entities/Factory/ComponentAssembler.h"
#include "Entities/Factory/EntitySerialiser.h"
#include "Graphics/Raycast.h"
#include "System/Files/ConfigManager.h"
#include "UIEntityBuilder.h"
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

static Entity CreateHealthBar(const EntityMetaData& emd)
{
	Entity entity = AssembleEntity(emd);
			
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

// something that.. acts?
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
		return AssembleEntity(meta_data, entity);
	}

	return AssembleEntity(emd, entity);
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

	// Animator // todo:remove?
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
	//if(ECS::Relic* relic = RelicRegistry::GetRelic("DiscountCardCost"))
	//{
	//	//inventory.relics.push_back(*relic);
	//}
	//if(ECS::Relic* relic = RelicRegistry::GetRelic("IncreaseColourDrawRate"))
	//{
	//	//inventory.relics.push_back(*relic);
	//}

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
				position = position - VectorF(40.0f, 0.0f);

			transform.SetWorldPosition( position );
		}
	}

	ASSERT(GetComponentRef(Transform, entity).size.isPositive(), "%s: Invalid Transform, has size 0", GetName(entity));
}

typedef Entity(*CreateEntityFn)(const EntityMetaData& emd);
static std::unordered_map<BasicString, CreateEntityFn> s_createEntitiyFunctions;

static void InitEntityFunctions()
{
	// game object entities
	s_createEntitiyFunctions["HealthBar"] = CreateHealthBar;
	s_createEntitiyFunctions["Enemy"] = CreateEnemy;
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

	Biome& biome = GetComponentRef(Biome, biome_entity);
	for (u32 i = 0; i < biome.entityMetaData.size(); i++)
	{
		const EntityMetaData& emd = biome.entityMetaData[i];
		Entity entity = CreateEntityFromData(emd);
	}
}
