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
		StartAnimation();
	}
	void IdleState::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		if(const ECS::Collider* collider = ecs->GetComponent(Collider, entity))
		{
			const FrameRateController& frc = FrameRateController::Get();
			if(collider->lastHitFrame != -1 && (collider->lastHitFrame + 20) >= frc.frameCount())
			{
				CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
				PushNewState(TakeHit);
			}
		}
		
		ECS::Physics& physics = ecs->GetComponentRef(Physics, entity);
		physics.ApplyDrag(0.5f);
		
		ECS::AIController& ai_controller = ecs->GetComponentRef(AIController, entity);
		if( ai_controller.moveToTarget ) // can move  to target
		{
			ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
			PushNewState(Run);
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
		StartAnimation();
	}

	void RunState::Update(float dt)
	{
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
		ECS::Physics& physics = ecs->GetComponentRef(Physics, entity);

		const VectorI facing_direction = state.GetFacingDirection();
		physics.maxSpeed.x = 4.0f;
		physics.ApplyMovement(facing_direction.toFloat(), dt);
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
		const Animator& animation = ecs->GetComponentRef(Animator, entity);

		if(animation.loopCount > 0)
		{
			CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
			state.actions.Pop();
			return;
		}
	}

	void TakeHitState::Exit()
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		if(ECS::Collider* collider = ecs->GetComponent(Collider, entity))
		{
			collider->lastHitFrame = -1;
		}
	}

	// DeathState
	// ---------------------------------------------------------
	void DeathState::Init()
	{
		StartAnimation();

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
		StartAnimation();
	}

	void DeathState::Update(float dt)
	{	
		EntityCoordinator* ecs = GameData::Get().ecs;
		const Animator& animation = ecs->GetComponentRef(Animator, entity);

		if(animation.loopCount > 0)
			can_kill = true;
	}
}


