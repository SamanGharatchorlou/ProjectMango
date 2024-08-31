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

		if (ECS::Physics* physics = ecs->GetComponent(Physics, entity))
		{
			physics->ApplyDrag(0.1f);
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


