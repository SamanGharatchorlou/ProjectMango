#include "pch.h"
#include "ComponentsSetup.h"

#include "ECS/Components/AIController.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Biome.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/PlayerController.h"
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
#include "ECS/EntSystems/HealthSystem.h"
#include "ECS/EntSystems/ComponentUpdateSystem.h"

void ECS::RegisterAllComponents()
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	
	ecs->RegisterComponent(EntityData, 32);
	ecs->RegisterComponent(Transform, 32);
	ecs->RegisterComponent(Sprite, 32);
	ecs->RegisterComponent(CharacterState, 32);
	ecs->RegisterComponent(PlayerController, 32);
	ecs->RegisterComponent(Physics, 32);
	ecs->RegisterComponent(Animator, 32);
	ecs->RegisterComponent(Collider, 32);
	ecs->RegisterComponent(AIController, 32);
	ecs->RegisterComponent(Pathing, 32);
	ecs->RegisterComponent(Damage, 32);
	ecs->RegisterComponent(Health, 32);
	ecs->RegisterComponent(Biome, 4);
	ecs->RegisterComponent(Spawner, 4);
}

void ECS::RegisterAllSystems()
{
	EntityCoordinator* ecs = GameData::Get().ecs;

	// Transform
	Signature transformSignature = ArcheBit(Transform);
	ecs->RegisterSystem<TransformSystem>(transformSignature);

	// Rendering
	Signature renderSignature = ArcheBit(Transform) | ArcheBit(Sprite);
	ecs->RegisterSystem<RenderSystem>(renderSignature);

	// Player Controller
	Signature playerInputSignature = ArcheBit(PlayerController) | ArcheBit(CharacterState) | ArcheBit(Physics);
	ecs->RegisterSystem<PlayerControllerSystem>(playerInputSignature);

	// Physics
	Signature physicsSignature = ArcheBit(Physics);
	ecs->RegisterSystem<PhysicsSystem>(physicsSignature);

	// Animation
	Signature animationSignature = ArcheBit(Sprite) | ArcheBit(Animator);
	ecs->RegisterSystem<AnimationSystem>(animationSignature);

	// todo: change name to BiomeSystem or something
	// Map
	Signature biomeSignature = ArcheBit(Biome);
	ecs->RegisterSystem<TileMapSystem>(biomeSignature);

	// Collisions
	Signature collisionSignature = ArcheBit(Collider);
	ecs->RegisterSystem<CollisionSystem>(collisionSignature);

	// AI Controller
	Signature AIControllerSignature = ArcheBit(AIController) | ArcheBit(CharacterState);
	ecs->RegisterSystem<AIControllerSystem>(AIControllerSignature);

	// Pathing
	Signature PathingSignature = ArcheBit(Pathing) | ArcheBit(AIController) | ArcheBit(CharacterState);
	ecs->RegisterSystem<PathingSystem>(PathingSignature);

	// Compoenent Updates - add any components into here that have an update function you want to run instead
	Signature ComponentsSignature = ArcheBit(Spawner);
	ecs->RegisterSystem<ComponentUpdateSystem>(ComponentsSignature);

}


void ECS::RemoveAllComponents(Entity entity)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ecs->RemoveComponent(EntityData, entity);
	ecs->RemoveComponent(Transform, entity);
	ecs->RemoveComponent(Sprite, entity);
	ecs->RemoveComponent(CharacterState, entity);
	ecs->RemoveComponent(PlayerController, entity);
	ecs->RemoveComponent(Physics, entity);
	ecs->RemoveComponent(Animator, entity);
	ecs->RemoveComponent(Biome, entity);
	ecs->RemoveComponent(Collider, entity);
	ecs->RemoveComponent(AIController, entity);
	ecs->RemoveComponent(Pathing, entity);
	ecs->RemoveComponent(Damage, entity);
	ecs->RemoveComponent(Health, entity);
}