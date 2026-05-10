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
#include "Entities/Registries/CardRegistry.h"
#include "Entities/Registries/MonsterRegistry.h"
#include "Entities/Registries/RelicRegistry.h"
#include "Entities/Registries/StatusEffectRegistry.h"
#include "Entities/States/Behaviours.h"




void ECS::RegisterAllComponents()
{
	COMPONENT_LIST(DEFINE_COMPONENT)

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
	Signature UISignature = ArcheBit(UIText) | ArcheBit(UIButton);
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
	// Compoenent Updates - run any random part update loops here if they dont need their own system
	Signature ComponentsSignature = 0;
	ecs->RegisterOrSystem<ComponentUpdateSystem>(ComponentsSignature);

	
	// --------- state systems ---------

	// AI Controller
	//Signature AIControllerSignature = ArcheBit(AIController);
	//ecs->RegisterAndSystem<AIControllerSystem>(AIControllerSignature);

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

	CardRegistry::ReadomFromCSV("Tier1Cards", 0);
	CardRegistry::ReadomFromJson("Tier2Cards", 1);
	CardRegistry::ReadomFromJson("Tier3Cards", 2);

	MonsterRegistry::Build( "CardMonsters" );

	RelicRegistry::PopulateRegistry();
	StatusEffectRegistry::PopulateRegistry();
}

void ECS::ClearComponentData()
{
	AnimationReader::ClearAnimationData();
	CardRegistry::ClearAll();
	MonsterRegistry::Clear();
}