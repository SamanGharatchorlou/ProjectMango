#include "pch.h"
#include "ComponentsSetup.h"

#include "Game/Initialiser.h"

#include "ECS/Components/AIController.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/UIComponents.h"
#include "ECS/Components/Biome.h"
#include "ECS/Components/Physics.h"
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
#include "ECS/EntSystems/UISystem.h"
#include "ECS/EntSystems/ComponentUpdateSystem.h"

void ECS::RegisterAllComponents()
{
	// should i define these outside somewhere more accessible for some reason?
	DEFINE_COMPONENT(EntityData, 32)
	DEFINE_COMPONENT(Transform, 32);
	DEFINE_COMPONENT(Sprite, 32);
	DEFINE_COMPONENT(CharacterState, 32);
	DEFINE_COMPONENT(PlayerController, 32);
	DEFINE_COMPONENT(Physics, 32);
	DEFINE_COMPONENT(Animator, 32);
	DEFINE_COMPONENT(Collider, 32);
	DEFINE_COMPONENT(AIController, 32);
	DEFINE_COMPONENT(Pathing, 32);
	DEFINE_COMPONENT(Damage, 32);
	DEFINE_COMPONENT(Health, 32);
	DEFINE_COMPONENT(Biome, 4);
	DEFINE_COMPONENT(Spawner, 4);
	DEFINE_COMPONENT(Door, 32);
	DEFINE_COMPONENT(UICursor, 1);

	ComponentInitialiser::InitAll();
}

void ECS::RemoveAllComponents(Entity entity)
{
	ComponentInitialiser::RemoveAll(entity);
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

	// UI
	Signature UISignature = ArcheBit(UICursor);
	ecs->RegisterSystem<UISystem>(UISignature);

	// Compoenent Updates - runs all basic object component update function (replace with having EITHER door, spawner etc....
	Signature ComponentsSignature = ArcheBit(Transform) | ArcheBit(Sprite) | ArcheBit(Animator);
	ecs->RegisterSystem<ComponentUpdateSystem>(ComponentsSignature);

}

