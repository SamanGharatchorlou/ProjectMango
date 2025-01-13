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
	Entity Create(const char* id, const char* config_id, VectorF spawn_pos)
	{
		Entity entity = Character::Create(id, config_id, spawn_pos);

		// CharacterState
		EntityCoordinator* ecs = GameData::Get().ecs;
		CharacterState& character_state = ecs->GetComponentRef(CharacterState, entity);
		character_state.character = new Enemy();
		character_state.config = config_id;

		return entity;
	}

	void Enemy::Begin(ECS::Entity entity)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

		if(!state.actions.HasAction())
			PushState(Idle);
	}

	bool Enemy::FinishedDying(ECS::Entity entity)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

		if(DeathState* death_state = static_cast<DeathState*>(&(state.actions.Top())))
			return death_state->can_kill;

		return false;
	}

	void Enemy::StartDying(ECS::Entity entity)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

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
		EntityCoordinator* ecs = GameData::Get().ecs;
		AIController& ai_controller = ecs->GetComponentRef(AIController, entity);
		bool can_flip_sprite = !ai_controller.cooldownTimer.IsRunning();
		
		StartAnimation(can_flip_sprite);
	}
	void IdleState::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		Physics& physics = ecs->GetComponentRef(Physics, entity);
		physics.ApplyDrag(0.05f);

		if( CanEnterHitState() )
		{
			CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
			PushState(TakeHit);
		}
		
		if( CoolingFromAttack() )
			return;

		if( CanMoveToTarget() )
		{
			CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
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
		EntityCoordinator* ecs = GameData::Get().ecs;
		AIController& ai_controller = ecs->GetComponentRef(AIController, entity);
		bool can_flip_sprite = !ai_controller.cooldownTimer.IsRunning();
		
		StartAnimation(can_flip_sprite);
	}

	void RunState::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		Transform& transform = ecs->GetComponentRef(Transform, entity);
		const AIController& ai_controller = ecs->GetComponentRef(AIController, entity);

		//if( ai_controller.cooldownTimer.IsRunning() )
		//{		
		//	Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
		//	sprite.canFlip = false;
		//	
		//	PopState();
		//	return;
		//}

		const int run_acceleration_factor = 1;
		ApplyMovementEase(run_acceleration_factor, dt);

		if(ai_controller.target != EntityInvalid && ecs->IsAlive(ai_controller.target))
		{
			const Transform& target_transform = ecs->GetComponentRef(Transform, ai_controller.target);
			const float distance_to_target = std::abs( transform.GetObjectCenter().x - target_transform.GetObjectCenter().x );
			const float attack_range = GetAttackRange(ActionState::BasicAttack) * 0.8f;

			if(attack_range > 0.0f && attack_range > distance_to_target)
			{
				CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
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
		EntityCoordinator* ecs = GameData::Get().ecs;
		const Animator& animator = ecs->GetComponentRef(Animator, entity);

		if(animator.loopCount > 0)
		{
			CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
			PopState();
			return;
		}
	}

	void TakeHitState::Exit()
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		if(Collider* collider = ecs->GetComponent(Collider, entity))
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
		EntityCoordinator* ecs = GameData::Get().ecs;
		const Animator& animation = ecs->GetComponentRef(Animator, entity);

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
		EntityCoordinator* ecs = GameData::Get().ecs;
		CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
		Animator& animator = ecs->GetComponentRef(Animator, entity);
		
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
			AIController& ai_controller = ecs->GetComponentRef(AIController, entity);
			ai_controller.cooldownTimer.Start();

			PopState();
			return;
		}

		if (ECS::Physics* physics = ecs->GetComponent(Physics, entity))
		{
			physics->ApplyDrag(0.5f);
		}
	}

	void BasicAttackState::Exit()
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		ecs->entities.KillEntity(attackCollider);
	}
}
