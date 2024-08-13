#include "pch.h"
#include "ComponentsSetup.h"

#include "ECS/Components/AIController.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/PlayerController.h"
#include "ECS/Components/TileMap.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/EntSystems/AIControllerSystem.h"
#include "ECS/EntSystems/AnimationSystem.h"
#include "ECS/EntSystems/CollisionSystem.h"
#include "ECS/EntSystems/PathingSystem.h"
#include "ECS/EntSystems/PhysicsSystem.h"
#include "ECS/EntSystems/PlayerControllerSystem.h"
#include "ECS/EntSystems/RenderSystem.h"
#include "ECS/EntSystems/TileMapSystem.h"
#include "ECS/EntSystems/TransformSystem.h"
#include "ECS/SystemManager.h"



void ECS::RegisterAllComponents()
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;

	ecs->RegisterComponent(Transform, 32);
	ecs->RegisterComponent(Sprite, 32);
	ecs->RegisterComponent(CharacterState, 32);
	ecs->RegisterComponent(PlayerController, 32);
	ecs->RegisterComponent(Physics, 32);
	ecs->RegisterComponent(Animation, 32);
	ecs->RegisterComponent(TileMap, 4);
	ecs->RegisterComponent(Collider, 32);
	ecs->RegisterComponent(AIController, 32);
	ecs->RegisterComponent(Pathing, 32);
	ecs->RegisterComponent(Damage, 32);
	ecs->RegisterComponent(Health, 32);
}

void ECS::RegisterAllSystems()
{
	EntityCoordinator* ecs = GameData::Get().ecs;

	// Transform
	Signature transformSignature = ArcheBit(Transform); // also want sprite here, but its the same as rendering
	ecs->RegisterSystem<TransformSystem>(transformSignature);

	// Rendering
	Signature renderSignature = ArcheBit(Transform) | ArcheBit(Sprite);
	ecs->RegisterSystem<RenderSystem>(renderSignature);

	// Player Controller
	Signature playerInputSignature = ArcheBit(PlayerController) | ArcheBit(CharacterState) | ArcheBit(Transform) | ArcheBit(Physics);
	ecs->RegisterSystem<PlayerControllerSystem>(playerInputSignature);

	// Physics
	Signature physicsSignature = ArcheBit(Transform) | ArcheBit(CharacterState) | ArcheBit(Physics);
	ecs->RegisterSystem<PhysicsSystem>(physicsSignature);

	// Animation
	Signature animationSignature = ArcheBit(Sprite) | ArcheBit(Animation) | ArcheBit(CharacterState) | ArcheBit(Transform);
	ecs->RegisterSystem<AnimationSystem>(animationSignature);

	// Map
	Signature mapTileSignature = ArcheBit(TileMap);
	ecs->RegisterSystem<TileMapSystem>(mapTileSignature);

	// Collisions
	Signature collisionSignature = ArcheBit(Collider) | ArcheBit(Transform);
	ecs->RegisterSystem<CollisionSystem>(collisionSignature);

	// AI Controller
	Signature AIControllerSignature = ArcheBit(AIController) | ArcheBit(CharacterState) | ArcheBit(Transform) | ArcheBit(Physics);
	ecs->RegisterSystem<AIControllerSystem>(AIControllerSignature);

	// Pathing
	Signature PathingSignature = ArcheBit(Pathing) | ArcheBit(Transform);
	ecs->RegisterSystem<PathingSystem>(PathingSignature);
}


void ECS::RemoveAllComponents(Entity entity)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ecs->RemoveComponent(Transform, entity);
	ecs->RemoveComponent(Sprite, entity);
	ecs->RemoveComponent(CharacterState, entity);
	ecs->RemoveComponent(PlayerController, entity);
	ecs->RemoveComponent(Physics, entity);
	ecs->RemoveComponent(Animation, entity);
	ecs->RemoveComponent(TileMap, entity);
	ecs->RemoveComponent(Collider, entity);
	ecs->RemoveComponent(AIController, entity);
	ecs->RemoveComponent(Pathing, entity);
	ecs->RemoveComponent(Damage, entity);
	ecs->RemoveComponent(Health, entity);
}