#include "pch.h"
#include "ShockSweeperEnemy.h"

#include "Core/Helpers.h"
#include "ECS/Components/AIController.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Physics.h"
#include "ECS/EntityCoordinator.h"
#include "Game/FrameRateController.h"

namespace ShockSweeper
{
	using namespace ECS;
	
	// Enemy
	// ---------------------------------------------------------
	Entity Create(const ECS::EntityMetaData& emd)
	{
		Entity entity = Character::CreateBasicEnemy(emd);

		const Config* config = ECS::GetConfigFromEntity(entity);

		// CharacterState
		CharacterState& character_state = AddComponent(CharacterState, entity);
		character_state.character = new Enemy();

		// Pathing
		bool disable_pathing = config->data.GetBool("disable_pathing", false);
		if(!disable_pathing)
		{
			Pathing& pathing = AddComponent(Pathing, entity);
			pathing.Init();
		}
		
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
		physics.ApplyHorizontalDrag(0.05f);

		if( CanEnterHitState() )
		{
			CharacterState& state = GetComponentRef(CharacterState, entity);
			PushState(TakeHit);
		}
		
		if( CoolingFromAttack() )
			return;
		
		const AIController& ai_controller = GetComponentRef(AIController, entity);
		if( ai_controller.canMoveToTarget )
		{
			CharacterState& state = GetComponentRef(CharacterState, entity);
			PushState(Run);
		}
	}

	// Run
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
		CharacterState& state = GetComponentRef(CharacterState, entity);

		if(ai_controller.canMoveToTarget)
		{
			Pathing& pathing = GetComponentRef(Pathing, entity);
			Physics& physics = GetComponentRef(Physics, entity);

			const int run_acceleration_factor = 1;
			const SDL_RendererFlip flip_direction = GetFacingDirection(entity);
			const VectorI facing_direction = FacingDirectionToVector(flip_direction);
			VectorF desired_movement = physics.GetMovementEase(facing_direction.toFloat(), dt, run_acceleration_factor);

			VectorF position = GetPosition(entity);
			pathing.targetLocation = position + desired_movement;
		}
		else
		{
			PopState();
		}

		const Transform& target_transform = GetComponentRef(Transform, ai_controller.target);
		const float distance_to_target = std::abs( transform.GetObjectCenter().x - target_transform.GetObjectCenter().x );
		const float attack_range = GetAttackRange(ActionState::BasicAttack) * 0.8f;

		if(attack_range > 0.0f && attack_range > distance_to_target)
		{
			CharacterState& state = GetComponentRef(CharacterState, entity);
			ReplaceState(BasicAttack);
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
		StartAnimation();
	}

	void BasicAttackState::Update(float dt)
	{
		CharacterState& state = GetComponentRef(CharacterState, entity);
		Animator& animator = GetComponentRef(Animator, entity);
		
		const Animation& animation = animator.GetActiveAnimation();
		if(animation.attackColliderFrameStart != -1)
		{
			if(animator.frameIndex >= animation.attackColliderFrameStart && attackCollider == EntityInvalid)
			{
				attackCollider = CreateNewAttackCollider("shock sweeper basic attack collider", 10.0f, 30.0f);
			}
		}

		if(animator.loopCount > 0)
		{
			AIController& ai_controller = GetComponentRef(AIController, entity);
			ai_controller.cooldownTimer.Start();

			PopState();
			return;
		}

		if (ECS::Physics* physics = GetComponent(Physics, entity))
		{
			physics->ApplyHorizontalDrag(0.5f);
		}
	}

	void BasicAttackState::Exit()
	{
		ecs->entities.KillEntity(attackCollider);
	}
}


namespace TrainingDummy
{
	using namespace ECS;
	
	// Enemy
	// ---------------------------------------------------------
	Entity Create(const EntityMetaData& emd)
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
		if( CanEnterHitState(c_hitFrameBuffer) )
		{
			CharacterState& state = GetComponentRef(CharacterState, entity);
			PushState(TakeHit);
		}
	}

	
	// TakeHitState
	// ---------------------------------------------------------
	void TakeHitState::Init()
	{
		StartAnimation();
		
		const FrameRateController& frc = FrameRateController::Get();
		frameStart = frc.FrameCount();
	}

	void TakeHitState::Update(float dt)
	{	
		
		const Animator& animator = GetComponentRef(Animator, entity);
		CharacterState& state = GetComponentRef(CharacterState, entity);

		if(animator.loopCount > 0)
		{
			PopState();
			return;
		}
		
		const FrameRateController& frc = FrameRateController::Get();
		if( (frameStart + c_hitFrameBuffer) < frc.FrameCount())
		{
			if(const Collider* collider = GetComponent(Collider, entity))
			{
				if(collider->lastHitFrame != -1 && (collider->lastHitFrame + 20) >= frc.FrameCount())
				{
					ReplaceState(TakeHit);
				}
			}
		}
	}

	void TakeHitState::Exit()
	{
		
		if(Collider* collider = GetComponent(Collider, entity))
		{
			//collider->lastHitFrame = -1;
		}
	}
}