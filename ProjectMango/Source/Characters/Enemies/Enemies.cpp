#include "pch.h"
#include "Enemies.h"

#include "ECS/Components/Components.h"
#include "ECS/Components/Collider.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/AIController.h"

#include "Graphics/TextureManager.h"
#include "System/Files/ConfigManager.h"
#include "Characters/States/PlayerStates.h"
#include "ECS/Components/Physics.h"
#include "Animations/AnimationReader.h"

static void ParseEnemyData(const char* file)
{
	BasicString full_path = FileManager::Get()->findFile(FileManager::Configs, "BasicEnemy");
	XMLParser* parser = new XMLParser;
	parser->parseXML(full_path.c_str());

	const XMLNode root = parser->rootNode();
	XMLNode transformNode = parser->rootChild("Transform");

	std::map<BasicString, VectorF> values;
	while (transformNode)
	{
		StringMap32 map;
		map.fillAtributes(transformNode);
		transformNode = transformNode.next();
	}
}

ECS::Entity Enemy::Create()
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;

	ECS::Entity entity = ecs->CreateEntity("Enemy");

	StringMap32 enemy_data;

	ParseEnemyData("BasicEnemy");

	// Transform
	ECS::Transform& transform = ecs->AddComponent(Transform, entity);
	transform.rect.SetSize(VectorF(17.0f, 34.0f));
	transform.positionOffset = VectorF(-8.0f, 26.0f);
	
	// MovementPhysics
	ECS::Physics& physics = ecs->AddComponent(Physics, entity);
	physics.applyGravity = false;	
	physics.acceleration = VectorF(100.0f, 100.0f);
	physics.maxSpeed = VectorF(5.0f, 5.0f);
	physics.speed = VectorF(0,2);

	// Animation
	ECS::Animation& animation = ecs->AddComponent(Animation, entity);
	AnimationReader::Parse( "TribeWarriorAnimations", animation.animator );
	animation.animator.start();



	// Sprite
	ECS::Sprite& sprite = ecs->AddComponent(Sprite, entity);
	sprite.renderLayer = 9;
	
	// Collider
	ECS::Collider& collider = ecs->AddComponent(Collider, entity);
	collider.SetRect(transform.rect);
	SetFlag<u32>(collider.mFlags, (u32)ECS::Collider::IsEnemy);
	
	// AI Controller
	ECS::AIController& ai_controller = ecs->AddComponent(AIController, entity);

	std::vector<ActionState> actions;
	for( u32 i = 0; i < (u32)ActionState::Count; i++ )
	{
		actions.push_back((ActionState)i);
	}
	ai_controller.statePool.load(actions, 4);
	
	// CharacterState
	ECS::CharacterState& character_state = ecs->AddComponent(CharacterState, entity);
	character_state.facingDirection = VectorI(0,1); // facing down

	// Pathing
	ecs->AddComponent(Pathing, entity);

	// Health
	ECS::Health& health = ecs->AddComponent(Health, entity);
	health.maxHealth = 100;
	health.currentHealth = 100;

	return entity;
}
