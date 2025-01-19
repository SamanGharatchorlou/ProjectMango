#include "pch.h"
#include "ComponentsSetup.h"

#include "Game/Initialiser.h"
#include "ECS/Components/AIController.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/SpellComponents.h"
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
#include "ECS/EntSystems/SpellSystem.h"
#include "ECS/EntSystems/ComponentUpdateSystem.h"

static constexpr u32 c_allEntities = 128;
static constexpr u32 c_veryCommon = 64 ;
static constexpr u32 c_common = 32;
static constexpr u32 c_uncommon = 16;
static constexpr u32 c_rare = 4;

void ECS::RegisterAllComponents()
{
	DEFINE_COMPONENT(Transform, c_allEntities);
	DEFINE_COMPONENT(Sprite, c_allEntities);
	DEFINE_COMPONENT(Collider, c_allEntities);

	DEFINE_COMPONENT(Physics, c_veryCommon);
	DEFINE_COMPONENT(Animator, c_veryCommon);
	DEFINE_COMPONENT(Health, c_veryCommon);

	DEFINE_COMPONENT(EntityData, c_common)
	DEFINE_COMPONENT(CharacterState, c_common);
	DEFINE_COMPONENT(AIController, c_common);
	DEFINE_COMPONENT(Pathing, c_common);
	DEFINE_COMPONENT(Damage, c_common);
	DEFINE_COMPONENT(Spell, c_common);

	DEFINE_COMPONENT(Door, c_uncommon);
	DEFINE_COMPONENT(Pickup, c_uncommon);
	DEFINE_COMPONENT(Spawner, c_uncommon);

	DEFINE_COMPONENT(PlayerController, c_rare);
	DEFINE_COMPONENT(Biome, c_rare);
	DEFINE_COMPONENT(UICursor, c_rare);
	DEFINE_COMPONENT(SpellBook, c_rare);

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

	// Spell
	Signature SpellSignature = ArcheBit(Spell);
	ecs->RegisterSystem<SpellSystem>(SpellSignature);

	// Compoenent Updates - runs all basic object component update function (replace with having EITHER door, spawner etc....
	Signature ComponentsSignature = ArcheBit(Transform) | ArcheBit(Sprite) | ArcheBit(Animator);
	ecs->RegisterSystem<ComponentUpdateSystem>(ComponentsSignature);
}

