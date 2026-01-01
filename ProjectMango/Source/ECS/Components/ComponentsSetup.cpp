#include "pch.h"
#include "ComponentsSetup.h"

#include "Game/Initialiser.h"
#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/EntSystems/AIControllerSystem.h"
#include "ECS/EntSystems/BehaviourSystem.h"
#include "ECS/EntSystems/AnimationSystem.h"
#include "ECS/EntSystems/CollisionSystem.h"
#include "ECS/EntSystems/PathingSystem.h"
#include "ECS/EntSystems/CardSystem.h"
#include "ECS/EntSystems/PhysicsSystem.h"
#include "ECS/EntSystems/SpawnSystem.h"
#include "ECS/EntSystems/StateResolutionSystem.h"
#include "ECS/EntSystems/PlayerControllerSystem.h"
#include "ECS/EntSystems/EntityStateSystem.h"
#include "ECS/EntSystems/RenderSystem.h"
#include "ECS/EntSystems/TileMapSystem.h"
#include "ECS/EntSystems/TransformSystem.h"
#include "ECS/EntSystems/UISystem.h"
#include "ECS/EntSystems/InputSystem.h"
#include "ECS/EntSystems/CallbackSystem.h"
#include "ECS/EntSystems/HealthSystem.h"
#include "ECS/EntSystems/TurnActionSystem.h"
#include "ECS/EntSystems/ComponentUpdateSystem.h"

#include "Game/Readers/AnimationReader.h"
#include "Entities/CardRegistry.h"
#include "Entities/MonsterRegistry.h"
#include "Entities/States/Behaviours.h"

static constexpr u32 c_allEntities = 128;
static constexpr u32 c_veryCommon = 64 ;
static constexpr u32 c_common = 32;
static constexpr u32 c_uncommon = 16;
static constexpr u32 c_rare = 4;

void ECS::RegisterAllComponents()
{
	// add an entry into EntityCommon.h
	DEFINE_COMPONENT(Transform, c_allEntities);
	DEFINE_COMPONENT(Sprite, c_allEntities);
	DEFINE_COMPONENT(Collider, c_allEntities);

	DEFINE_COMPONENT(Physics, c_veryCommon);
	DEFINE_COMPONENT(Animator, c_veryCommon);
	DEFINE_COMPONENT(Health, c_veryCommon);
	DEFINE_COMPONENT(Audio, c_veryCommon);
	DEFINE_COMPONENT(Card, c_uncommon);

	DEFINE_COMPONENT(EntityData, c_common)
	DEFINE_COMPONENT(EntityState, c_common);
	DEFINE_COMPONENT(AIController, c_common);
	DEFINE_COMPONENT(Pathing, c_common);
	DEFINE_COMPONENT(Faction, c_common);
	DEFINE_COMPONENT(AIIntent, c_common);
	DEFINE_COMPONENT(BehaviourMap, c_common);
	DEFINE_COMPONENT(BehaviourState, c_common);
	DEFINE_COMPONENT(Damage, c_common);
	DEFINE_COMPONENT(DeathScentence, c_common);
	DEFINE_COMPONENT(UIText, c_uncommon);
	DEFINE_COMPONENT(Callback, c_uncommon);
	DEFINE_COMPONENT(Colour, c_uncommon);

	DEFINE_COMPONENT(SpawnRequest, c_uncommon);
	DEFINE_COMPONENT(UIButton, c_uncommon);
	DEFINE_COMPONENT(LayeredSprite, c_uncommon);
	DEFINE_COMPONENT(SpriteCycle, c_uncommon);
	DEFINE_COMPONENT(SpriteSheet, c_uncommon);
	
	DEFINE_COMPONENT(CoinStack, Colour::Count);
	
	DEFINE_COMPONENT(Spawner, c_rare);
	DEFINE_COMPONENT(PlayerController, c_rare);
	DEFINE_COMPONENT(Biome, c_rare);
	DEFINE_COMPONENT(Inventory, c_rare);
	DEFINE_COMPONENT(TurnState, c_rare);
	DEFINE_COMPONENT(ActionRequest, c_rare);
	DEFINE_COMPONENT(Jiggler, c_rare);

	DEFINE_COMPONENT(UICursor, 1);

	ComponentInitialiser::InitAll();
}

void ECS::RemoveAllComponents(Entity entity)
{
	ComponentInitialiser::RemoveAll(entity);
}

void ECS::RegisterAllSystems()
{
	// --------- input/UI systems ---------
	
	// UI
	Signature UISignature = ArcheBit(UIText) | ArcheBit(UIButton) | ArcheBit(Card);
	ecs->RegisterOrSystem<UISystem>(UISignature);

	// Input
	Signature InputSignature = ArcheBit(UIButton);
	ecs->RegisterOrSystem<InputSystem>(InputSignature);
	
	// Callbacks
	Signature CallbackSignature = ArcheBit(Callback);
	ecs->RegisterOrSystem<CallbackSystem>(CallbackSignature);


	// --------- higher-level systems ---------
	
	// TurnActionSystem
	Signature turnActionSignature = ArcheBit(TurnState);
	ecs->RegisterAndSystem<TurnActionSystem>(turnActionSignature); 

	// Player Controller
	Signature playerInputSignature = ArcheBit(PlayerController);
	ecs->RegisterAndSystem<PlayerControllerSystem>(playerInputSignature);




	// --------- gameplay-logic systems ---------

	// Health
	Signature HealthSignature = ArcheBit(Health) | ArcheBit(DeathScentence);
	ecs->RegisterOrSystem<HealthSystem>(HealthSignature);

	// Spawn
	Signature SpawnSignature = ArcheBit(Spawner);
	ecs->RegisterAndSystem<SpawnSystem>(SpawnSignature);

	// Spawn
	Signature cardSignature = ArcheBit(Card);
	ecs->RegisterAndSystem<CardSystem>(cardSignature);

	// todo: can remove this now?
	// Compoenent Updates - runs all basic object component update function
	Signature ComponentsSignature = ArcheBit(Jiggler);
	ecs->RegisterOrSystem<ComponentUpdateSystem>(ComponentsSignature);

	
	// --------- state systems ---------

	// AI Controller
	Signature AIControllerSignature = ArcheBit(AIController);
	ecs->RegisterAndSystem<AIControllerSystem>(AIControllerSignature);

	// State Resolution
	Signature StateResolutionSignature = ArcheBit(EntityState) | ArcheBit(AIIntent) | ArcheBit(BehaviourState);
	ecs->RegisterAndSystem<StateResolutionSystem>(StateResolutionSignature);

	// State Transision
	Signature EntityStateSignature = ArcheBit(EntityState);
	ecs->RegisterAndSystem<EntityStateSystem>(EntityStateSignature);

	// Animation
	Signature animationSignature = ArcheBit(Animator);
	ecs->RegisterAndSystem<AnimationSystem>(animationSignature);

	// Behaviour
	Signature BehaviourSignature = ArcheBit(BehaviourMap) | ArcheBit(EntityState) | ArcheBit(BehaviourState);
	ecs->RegisterAndSystem<BehaviourSystem>(BehaviourSignature);

	// Pathing
	Signature PathingSignature = ArcheBit(Pathing) | ArcheBit(AIController) | ArcheBit(EntityState);
	ecs->RegisterAndSystem<PathingSystem>(PathingSignature);

	
	// --------- physics systems ---------

	// Physics
	Signature physicsSignature = ArcheBit(Physics);
	ecs->RegisterAndSystem<PhysicsSystem>(physicsSignature);

	// Collisions
	Signature collisionSignature = ArcheBit(Collider);
	ecs->RegisterAndSystem<CollisionSystem>(collisionSignature);

	// Transform
	Signature transformSignature = ArcheBit(Transform);
	ecs->RegisterAndSystem<TransformSystem>(transformSignature);


	// --------- rendering systems ---------

	// Biome (this is basically just rendering right now)
	Signature biomeSignature = ArcheBit(Biome);
	ecs->RegisterAndSystem<TileMapSystem>(biomeSignature);

	// Rendering
	Signature renderSignature = ArcheBit(Sprite) | ArcheBit(UIText) | ArcheBit(LayeredSprite);
	ecs->RegisterOrSystem<RenderSystem>(renderSignature);
}


void ECS::ParseComponentData()
{
	// parse all the animation data here too, bank it, then read from it rather than parse it everytime
	AnimationReader::ReadAnimationData();

	CardRegistry::Build("Tier1Cards", 0);
	CardRegistry::Build("Tier2Cards", 1);
	CardRegistry::Build("Tier3Cards", 2);

	MonsterRegistry::Build( "Monsters" );
}