#include "pch.h"
#include "EnemyStates.h"

#include "Core/Helpers.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "ECS/Components/AIController.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/ComponentCommon.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Physics.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/EntSystems/AnimationSystem.h"
#include "Game/States/State.h"
#include "Game/FrameRateController.h"

namespace Enemy
{
	using namespace ECS;

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
		AIController& ai_controller = ecs->GetComponentRef(AIController, entity);

		if(const Collider* collider = ecs->GetComponent(Collider, entity))
		{
			const FrameRateController& frc = FrameRateController::Get();
			if(collider->lastHitFrame != -1 && (collider->lastHitFrame + 20) >= frc.FrameCount())
			{
				CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
				PushState(TakeHit);
			}
		}
		
		Physics& physics = ecs->GetComponentRef(Physics, entity);
		physics.ApplyDrag(0.05f);
		
		if( ai_controller.cooldownTimer.IsRunning() )
		{
			if(ai_controller.cooldownTimer.GetSeconds() < ai_controller.attackCooldownTime)
				return;
			
			ai_controller.cooldownTimer.Stop();
		}
		
		if( ai_controller.moveToTarget ) // can move  to target
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
		CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
		Physics& physics = ecs->GetComponentRef(Physics, entity);
		Transform& transform = ecs->GetComponentRef(Transform, entity);
		const AIController& ai_controller = ecs->GetComponentRef(AIController, entity);

		if( ai_controller.cooldownTimer.IsRunning() )
		{		
			Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
			sprite.canFlip = false;
			
			PopState();
			return;
		}
		
		const int run_acceleration_factor = 1;
		const VectorI facing_direction = state.GetFacingDirection();
		physics.ApplyMovementEase(facing_direction.toFloat(), dt, run_acceleration_factor);

		if(ai_controller.target != EntityInvalid)
		{
			const Transform& target_transform = ecs->GetComponentRef(Transform, ai_controller.target);
			const float distance_to_target = std::abs( transform.GetObjectCenter().x - target_transform.GetObjectCenter().x );
			const float attack_range = GetAttackRange(ActionState::BasicAttack);

			if(attack_range > 0.0f && attack_range > distance_to_target)
			{
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

		EntityCoordinator* ecs = GameData::Get().ecs;

		if(Collider* collider = ecs->GetComponent(Collider, entity))
		{
			SetFlag(collider->flags, (u32)Collider::IgnoreAll);
		}

		if(Physics* physics = ecs->GetComponent(Physics, entity))
		{
			physics->speed.set(0.0f, 0.0f);
		}
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
		StartAnimation(true);
	}

	void BasicAttackState::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
		Animator& animator = ecs->GetComponentRef(Animator, entity);
		
		Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
		sprite.canFlip = animator.frameIndex < 2;

		if(animator.frameIndex >= 6 && attackCollider == EntityInvalid)
		{
			attackCollider = CreateNewAttackCollider("enemy attack collider", 10.0f, 30.0f);
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


