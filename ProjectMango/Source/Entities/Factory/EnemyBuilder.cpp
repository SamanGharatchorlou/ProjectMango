#include "pch.h"
#include "EnemyBuilder.h"

#include "Core/Helpers.h"
#include "Debugging/ImGui/ImGuiMenu.h"
#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Entities/States/Behaviours.h"
#include "EntityBuilder.h"
#include "Game/States/GameState.h"

using namespace ECS;

static void BuildIntentIconEntity(Entity icon_entity)
{
	UIIntentIcon& icon = AddComponent(UIIntentIcon, icon_entity);
	icon.displays.push_back({ "basic_approach_icon"	, EnemyPhase::Approach });
	icon.displays.push_back({ "basic_attack_icon"	, EnemyPhase::Attack });
	icon.displays.push_back({ "basic_recovery_icon"	, EnemyPhase::Recover });
	icon.displays.push_back({ "debuff_icon"			, EnemyPhase::Debuff });

	// Transform
	Transform& child_transform = AddComponent(Transform, icon_entity);
	VectorF size = VectorF(16, 16);
	child_transform.size = AdjustToScreenSize(size);

	Entity parent = GetParent(icon_entity);
	const Transform& parent_transform = GetComponentRef(Transform, parent);
	VectorF object_tc = parent_transform.GetObjectRect().TopCenter();
	VectorF top_left = parent_transform.GetRect().TopLeft();

	VectorF local_position = VectorF(object_tc.x - top_left.x, (object_tc.y - top_left.y) * 0.25f);
	child_transform.SetLocalPosition(local_position);

	// Sprite
	Sprite& child_sprite = AddComponent(Sprite, icon_entity);
	child_sprite.params.renderLayer = RenderLayer::UI;
	child_sprite.params.colourMod.setOpacity(0.85f);
}

static void PopulateStrategy(const char* enemy_type, AIStrategy& strategy)
{
	EnemyPhase recover{ EnemyPhase::Recover, 1 };
	EnemyPhase attack{ EnemyPhase::Attack, 1 };
	EnemyPhase debuff{ EnemyPhase::Debuff, 1 };

	Strategy attack_pattern;
	attack_pattern.name = "Attack";

	if (StringCompare(enemy_type, "ShockSweeper"))
	{
		attack_pattern.phases.push_back(attack);
		attack_pattern.phases.push_back(recover);
		attack_pattern.phases.push_back(recover);
	}

	attack_pattern.phases.push_back(attack);
	attack_pattern.phases.push_back(recover);
	strategy.strategies.push_back(attack_pattern);

	if ( StringCompare(enemy_type, "OrbMage") )
	{
		Strategy debuff_pattern;
		debuff_pattern.name = "Debuff";
		debuff_pattern.phases.push_back(recover);
		debuff_pattern.phases.push_back(debuff);
		debuff_pattern.phases.push_back(recover);
		strategy.strategies.push_back(debuff_pattern);
	}

	strategy.currentPhase = 0;
	strategy.turnsLeft = 1;
}

static EntityMetaData s_enemyMetaData;

ECS::Entity CreateEnemy(const char* enemy_type)
{
	s_enemyMetaData.data.strings["enemy_type"] = enemy_type;
	return CreateEnemy(s_enemyMetaData);
}

// Enemy
// ---------------------------------------------------------
// create an actual enemy i.e. the thing the player fights
Entity CreateEnemy(const ECS::EntityMetaData& emd)
{
	s_enemyMetaData = emd;

	// pick random enemy create enemy registry
	const char* enemy_type = emd.data.GetString("enemy_type");
	Entity entity = CreateActor(emd, enemy_type);

	AddComponent(AIIntent, entity);

	// AIStrategy
	AIStrategy& strategy = AddComponent(AIStrategy, entity);
	PopulateStrategy(enemy_type, strategy);

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

	// Turn
	TurnState& turn = AddComponent(TurnState, entity);
	turn.initiative = 10;

	// mark ourselves as the enemy
	if (GameState* game_state = GameState::GetActive())
	{
		game_state->enemy = entity;
	}

	if (DebugMenu::GetSelectedEntity() == EntityInvalid)
		DebugMenu::SelectEntity(entity);

	return entity;
}
