#include "pch.h"
#include "BlindingSpiderEnemy.h"

#include "ECS/Components/AIController.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Physics.h"
#include "ECS/EntityCoordinator.h"
#include "System/Files/Config.h"
#include "ECS/Components/Biome.h"

namespace BlindingSpider
{
	using namespace ECS;
	
	// Enemy
	// ---------------------------------------------------------
	Entity Create(const ECS::EntityMetaData& emd)
	{
		Entity entity = Character::CreateBasicEnemy(emd);
				
		// CharacterState
		CharacterState& character_state = AddComponent(CharacterState, entity);
		character_state.character = new Enemy();
		
		return entity;
	}
	
	void Enemy::Begin(ECS::Entity entity)
	{
		CharacterState& state = GetComponentRef(CharacterState, entity);

		if(!state.actions.HasAction())
			PushState(Idle);
	}

	bool Enemy::FinishedDying(ECS::Entity entity)
	{
		CharacterState& state = GetComponentRef(CharacterState, entity);

		if(DeathState* death_state = static_cast<DeathState*>(&(state.actions.Top())))
			return death_state->can_kill;

		return false;
	}

	void Enemy::StartDying(ECS::Entity entity)
	{
		CharacterState& state = GetComponentRef(CharacterState, entity);

		state.actions.Pop();
		state.actions.Push( new DeathState(entity) );
		state.actions.Push( new TakeHitState(entity) );
	}

	// Idle
	// ---------------------------------------------------------
	void IdleState::Init()
	{
		StartAnimation();
	}
	void IdleState::Resume()
	{		
		AIController& ai_controller = GetComponentRef(AIController, entity);
		bool can_flip_sprite = !ai_controller.cooldownTimer.IsRunning();
		
		StartAnimation(can_flip_sprite);
	}
	void IdleState::Update(float dt)
	{
		Physics& physics = GetComponentRef(Physics, entity);
		physics.ApplyDrag(0.05f);

		if( CanEnterHitState() )
		{
			CharacterState& state = GetComponentRef(CharacterState, entity);
			PushState(TakeHit);
		}
		
		if( CoolingFromAttack() )
			return;

		//if( CanMoveToTarget() )
		{
			CharacterState& state = GetComponentRef(CharacterState, entity);
			PushState(Run);
		}
	}

	// Run -- This should be RunToTarget and then add a Partol state
	// ---------------------------------------------------------
	RunState::RunState(ECS::Entity _entity) : CharacterAction(ActionState::Run, _entity) { }

	void RunState::Init()
	{
		StartAnimation();
	}
	void RunState::Resume()
	{		
		
		AIController& ai_controller = GetComponentRef(AIController, entity);
		bool can_flip_sprite = !ai_controller.cooldownTimer.IsRunning();
		
		StartAnimation(can_flip_sprite);
	}

	void RunState::Update(float dt)
	{
		Transform& transform = GetComponentRef(Transform, entity);
		const AIController& ai_controller = GetComponentRef(AIController, entity);
		
		const int run_acceleration_factor = 1;

		if(ai_controller.CanMoveForward(run_acceleration_factor, dt))
		{
			ApplyMovementEase(run_acceleration_factor, dt);
		}
		else
		{		
			//if(!ai_controller.isAlert)
			{
				// turn around
				//ai_controller.
				//CharacterState& state = GetComponentRef(CharacterState, entity);
				//state.FlipFacingDirection();
				return;
			}
		}

		if(ai_controller.target != EntityInvalid && ecs->IsAlive(ai_controller.target))
		{
			const Transform& target_transform = GetComponentRef(Transform, ai_controller.target);
			const float distance_to_target = std::abs( transform.GetObjectCenter().x - target_transform.GetObjectCenter().x );
			const float attack_range = GetAttackRange(ActionState::BasicAttack);

			if(attack_range > 0.0f && attack_range > distance_to_target)
			{
				CharacterState& state = GetComponentRef(CharacterState, entity);
				ReplaceState(BasicAttack);
			}
		}
	}

	// TakeHitState
	// ---------------------------------------------------------
	void TakeHitState::Init()
	{
		StartAnimation();
	}

	void TakeHitState::Update(float dt)
	{	
		const Animator& animator = GetComponentRef(Animator, entity);

		if(animator.loopCount > 0)
		{
			CharacterState& state = GetComponentRef(CharacterState, entity);
			PopState();
			return;
		}
	}

	void TakeHitState::Exit()
	{
		if(Collider* collider = GetComponent(Collider, entity))
		{
			collider->lastHitFrame = -1;
		}
	}

	// DeathState
	// ---------------------------------------------------------
	void DeathState::Init()
	{
		StartAnimation(false);

		can_kill = false;

		InitDeathState();
	}

	void DeathState::Resume()
	{
		StartAnimation(false);
	}

	void DeathState::Update(float dt)
	{	
		const Animator& animation = GetComponentRef(Animator, entity);

		if(animation.loopCount > 0)
			can_kill = true;
	}

	// BasicAttack
	// ---------------------------------------------------------
	BasicAttackState::BasicAttackState(ECS::Entity _entity) : CharacterAction(ActionState::BasicAttack, _entity) { }

	void BasicAttackState::Init()
	{	
		Animator& animator = GetComponentRef(Animator, entity);
		if(animator.GetAnimation(ActionState::AttackWindUp))
		{
			StartAnimation(ActionState::AttackWindUp);
		}
		else
		{	
			StartAnimation();
		}
	}

	// THIS WHOLE THING IS FUCKED
	void BasicAttackState::Update(float dt)
	{
		Animator& animator = GetComponentRef(Animator, entity);
		
		const Animation& animation = animator.GetActiveAnimation();

		//if(attackCollider == EntityInvalid)
		//{					
		//	const CharacterState& state = GetComponentRef(CharacterState, entity);
		//	const Config* config = GetConfig(entity);
		//	attackCollider = CreateNewAttackCollider("player attack collider", config->values.GetFloat("basic_attack_damage"), config->values.GetFloat("basic_attack_force"));
		//}

		if(animator.loopCount > 0)
		{
			if(animation.action == ActionState::AttackWindUp)
			{
				StartAnimation();

				if(attackCollider == EntityInvalid)
				{
					const CharacterState& state = GetComponentRef(CharacterState, entity);
					const Config* config = GetConfig(entity);
					attackCollider = CreateNewAttackCollider("player attack collider", config->values.GetFloat("basic_attack_damage"), config->values.GetFloat("basic_attack_force"));
				}

				return;
			}

			AIController& ai_controller = GetComponentRef(AIController, entity);
			ai_controller.cooldownTimer.Start();
			
			CharacterState& state = GetComponentRef(CharacterState, entity);
			PopState();
			return;
		}

		if (ECS::Physics* physics = GetComponent(Physics, entity))
		{
			physics->ApplyDrag(0.5f);
		}
	}

	void BasicAttackState::Exit()
	{
		ecs->entities.KillEntity(attackCollider);
	}
}


