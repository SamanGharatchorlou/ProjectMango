#include "pch.h"
#include "BlindingSpiderEnemy.h"

#include "ECS/Components/AIComponents.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "System/Files/Config.h"
#include "ECS/Components/Biome.h"
#include "ECS/Components/Physics.h"
#include "Entities/EntityBuilder.h"

namespace BlindingSpider
{
	using namespace ECS;
	
	// Enemy
	// ---------------------------------------------------------
	Entity Create(const ECS::EntityMetaData& emd)
	{
		Entity entity = CreateActor(emd);
				
		// EntityState
		//EntityState& character_state = AddComponent(EntityState, entity);
		//character_state.character = new Enemy();
		
		return entity;
	}
	
	//void Enemy::Begin(ECS::Entity entity)
	//{
	//	EntityState& state = GetComponentRef(EntityState, entity);

	//	PushState(Idle);
	//}

	//bool Enemy::FinishedDying(ECS::Entity entity)
	//{
	//	EntityState& state = GetComponentRef(EntityState, entity);

	//	if(DeathState* death_state = static_cast<DeathState*>(&(state.actions.Top())))
	//		return death_state->can_kill;

	//	return false;
	//}

	//void Enemy::StartDying(ECS::Entity entity)
	//{
	//	EntityState& state = GetComponentRef(EntityState, entity);

	//	state.actions.Pop();
	//	state.actions.Push( new DeathState(entity) );
	//	state.actions.Push( new TakeHitState(entity) );
	//}

	//void Enemy::SpawnIn(ECS::Entity entity)
	//{
	//	EntityState& state = GetComponentRef(EntityState, entity);

	//	while(state.actions.HasAction())
	//		state.actions.Pop();

	//	// adds a pointer to the stack
	//	state.actions.Push( new SpawningState(entity) );
	//}

	//// Idle
	//// ---------------------------------------------------------
	//void IdleState::Init()
	//{
	//	StartAnimation();
	//}
	//void IdleState::Resume()
	//{		
	//	AIController& ai_controller = GetComponentRef(AIController, entity);
	//	bool can_flip_sprite = !ai_controller.cooldownTimer.IsRunning();
	//	
	//	StartAnimation(can_flip_sprite);
	//}
	//void IdleState::Update(float dt)
	//{
	//	const AIController& ai_controller = GetComponentRef(AIController, entity);
	//	if( ai_controller.canMoveToTarget )
	//	{
	//		EntityState& state = GetComponentRef(EntityState, entity);
	//		PushState(Run);
	//	}
	//}

	//// Spawning
	//// ---------------------------------------------------------
	//void SpawningState::Init()
	//{
	//	StartAnimation(ActionState::Idle);
	//}


	//// Run -- This should be RunToTarget and then add a Partol state
	//// ---------------------------------------------------------
	//RunState::RunState(ECS::Entity _entity) : CharacterAction(ActionState::Run, _entity) { }

	//void RunState::Init()
	//{
	//	StartAnimation();
	//}
	//void RunState::Resume()
	//{		
	//	AIController& ai_controller = GetComponentRef(AIController, entity);
	//	bool can_flip_sprite = !ai_controller.cooldownTimer.IsRunning();
	//	
	//	StartAnimation(can_flip_sprite);
	//}

	//void RunState::Update(float dt)
	//{
	//	Transform& transform = GetComponentRef(Transform, entity);

	//	Physics& physics = GetComponentRef(Physics, entity);

	//	const AIController& ai_controller = GetComponentRef(AIController, entity);
	//	
	//	const int run_acceleration_factor = 1;
	//	const SDL_RendererFlip flip_direction = GetFacingDirection(entity);
	//	const VectorI facing_direction = FacingDirectionToVector(flip_direction);

	//	if(ai_controller.CanMoveForward(run_acceleration_factor, dt))
	//	{
	//		physics.ApplyMovementEase(facing_direction.toFloat(), dt, 0.1);
	//	}


	//	// this adds to is (not sure if correct but at least some what correct)
	//	VectorF desired_movement = physics.speed;// physics.GetMovementEase(facing_direction.toFloat(), dt, 0.1);

	//	VectorF position = GetPosition(entity);
	//	Pathing& pathing = GetComponentRef(Pathing, entity);
	//	pathing.targetLocation = position + desired_movement;
	//	
	//	const EntityState& state = GetComponentRef(EntityState, entity);
	//	const Transform& target_transform = GetComponentRef(Transform, state.target);
	//	const float distance_to_target = std::abs( transform.GetObjectCenter().x - target_transform.GetObjectCenter().x );
	//	const float attack_range = GetAttackRange(ActionState::BasicAttack) * 0.8f;

	//	if(attack_range > 0.0f && attack_range > distance_to_target)
	//	{
	//		EntityState& state = GetComponentRef(EntityState, entity);
	//		ReplaceState(BasicAttack);
	//	}
	//}

	//// TakeHitState
	//// ---------------------------------------------------------
	//void TakeHitState::Init()
	//{
	//	StartAnimation();
	//}

	//void TakeHitState::Update(float dt)
	//{	
	//	const Animator& animator = GetComponentRef(Animator, entity);

	//	if(animator.loopCount > 0)
	//	{
	//		EntityState& state = GetComponentRef(EntityState, entity);
	//		PopState();
	//		return;
	//	}
	//}

	//void TakeHitState::Exit()
	//{

	//}

	//// DeathState
	//// ---------------------------------------------------------
	//void DeathState::Init()
	//{
	//	StartAnimation(false);

	//	can_kill = false;

	//	InitDeathState();
	//}

	//void DeathState::Resume()
	//{
	//	StartAnimation(false);
	//}

	//void DeathState::Update(float dt)
	//{	
	//	const Animator& animation = GetComponentRef(Animator, entity);

	//	if(animation.loopCount > 0)
	//		can_kill = true;
	//}

	//// BasicAttack
	//// ---------------------------------------------------------
	//BasicAttackState::BasicAttackState(ECS::Entity _entity) : CharacterAction(ActionState::BasicAttack, _entity) { }

	//void BasicAttackState::Init()
	//{	
	//	Animator& animator = GetComponentRef(Animator, entity);
	//	if(animator.GetAnimation(ActionState::AttackWindUp))
	//	{
	//		StartAnimation(ActionState::AttackWindUp);
	//	}
	//	else
	//	{	
	//		StartAnimation();
	//	}
	//}

	//// THIS WHOLE THING IS FUCKED
	//void BasicAttackState::Update(float dt)
	//{
	//	Animator& animator = GetComponentRef(Animator, entity);
	//	
	//	const Animation& animation = animator.GetActiveAnimation();

	//	//if(attackCollider == EntityInvalid)
	//	//{					
	//	//	const EntityState& state = GetComponentRef(EntityState, entity);
	//	//	const Config* config = GetConfig(entity);
	//	//	attackCollider = CreateNewAttackCollider("player attack collider", config->values.GetFloat("basic_attack_damage"), config->values.GetFloat("basic_attack_force"));
	//	//}

	//	if(animator.loopCount > 0)
	//	{
	//		if(animation.action == ActionState::AttackWindUp)
	//		{
	//			StartAnimation();

	//			//if(attackCollider == EntityInvalid)
	//			//{
	//			//	const EntityState& state = GetComponentRef(EntityState, entity);
	//			//	const Config* config = GetConfigFromEntity(entity);
	//			//	attackCollider = CreateNewAttackCollider("player attack collider", config->data.GetFloat("basic_attack_damage"), config->data.GetFloat("basic_attack_force"));
	//			//}

	//			return;
	//		}

	//		AIController& ai_controller = GetComponentRef(AIController, entity);
	//		ai_controller.cooldownTimer.Start();
	//		
	//		EntityState& state = GetComponentRef(EntityState, entity);
	//		PopState();
	//		return;
	//	}

	//}

	//void BasicAttackState::Exit()
	//{
	//	ecs->entities.KillEntity(attackCollider);
	//}
}


