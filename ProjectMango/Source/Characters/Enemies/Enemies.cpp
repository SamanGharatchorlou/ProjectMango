#include "pch.h"
#include "Enemies.h"

#include "ECS/Components/Components.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Level.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/AIController.h"
#include "ECS/EntityCoordinator.h"
#include "Graphics/TextureManager.h"
#include "Core/Helpers.h"

#include "Animations/AnimationReader.h"
#include "ECS/Components/Animator.h"

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

	// Transform
	ECS::Transform& transform = ecs->AddComponent(Transform, entity);
	transform.SetWorldPosition(ECS::Level::GetSpawnPos("TrainingDummy"));
	transform.size = VectorF(352, 120);

	// MovementPhysics
	ECS::Physics& physics = ecs->AddComponent(Physics, entity);
	physics.applyGravity = true;		
	physics.acceleration = VectorF(80.0f, 80.0f);
	physics.maxSpeed = VectorF(15.0f, 20.0f);

	ECS::Animator& animation = ecs->AddComponent(Animator, entity);
	AnimationReader::BuildAnimatior( "ShockSweeperAnimations", animation.animations );
	animation.activeAnimation = 0;

	// Sprite
	ECS::Sprite& sprite = ecs->AddComponent(Sprite, entity);
	sprite.renderLayer = 9;
	
	// Collider
	ECS::Collider& collider = ecs->AddComponent(Collider, entity);
	collider.SetBaseRect(RectF(VectorF::zero(), transform.size));
	collider.SetFlag(ECS::Collider::IsEnemy);

	// Health
	ECS::Health& health = ecs->AddComponent(Health, entity);
	health.maxHealth = 10000;
	health.currentHealth = health.maxHealth;

	// AI
	ECS::AIController& ai = ecs->AddComponent(AIController, entity);
	
	// Pathing
	ECS::Pathing& pathing = ecs->AddComponent(Pathing, entity);
	
	// CharacterState
	ECS::CharacterState& character_state = ecs->AddComponent(CharacterState, entity);

	return entity;
}
