#include "pch.h"
#include "PlayerStates.h"

#include "Animations/CharacterStates.h"
#include "Game/FrameRateController.h"
#include "Input/InputManager.h"

#include "Core/Helpers.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/ComponentCommon.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/PlayerController.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/EntSystems/AnimationSystem.h"

using namespace Player;
using namespace ECS;

static const int c_defaultEasingSpeed = 3;

// Idle
// ---------------------------------------------------------
IdleState::IdleState(Entity _entity) : CharacterAction(ActionState::Idle, _entity) { }

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
	CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

	// Run
	if(!state.movementInput.isZero())
	{
		PushState(Run);
	}
	else
	{
		if (Physics* physics = ecs->GetComponent(Physics, entity))
		{
			physics->ApplyDrag(0.6f);
		}
	}

	// Basic Attack
	InputManager* input = InputManager::Get();
	if (input->isCursorPressed(Cursor::ButtonType::Left, c_inputBuffer) || input->isCursorHeld(Cursor::ButtonType::Left))
	{
		PushState(BasicAttack);
	}

	if (input->isPressed(Button::Space, c_inputBuffer))
	{
		PushState(Jump);
	}
}

// Run
// ---------------------------------------------------------
RunState::RunState(Entity _entity) : CharacterAction(ActionState::Run, _entity) { }

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
	EntityCoordinator* ecs = GameData::Get().ecs;
	CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
	Physics& physics = ecs->GetComponentRef(Physics, entity);
	
	if(state.movementInput.isZero())
	{
		PopState();
		return;
	}

	const int run_acceleration_factor = 3;
	physics.maxSpeed.x = 800.0f;
	physics.ApplyMovementEase(state.movementInput.toFloat(), dt, run_acceleration_factor);
	
	InputManager* input = InputManager::Get();
	if (input->isPressed(Button::Space, c_inputBuffer))
	{
		PushState(Jump);
	}

	if (input->isPressed(Button::Shift, c_inputBuffer))
	{
		PushState(Roll);
	}

	if (input->isCursorPressed(Cursor::ButtonType::Left, c_inputBuffer) || input->isCursorHeld(Cursor::ButtonType::Left))
	{
		ReplaceState(BasicAttack);
	}
}


// JumpState
// ---------------------------------------------------------
JumpState::JumpState(Entity _entity) : CharacterAction(ActionState::Jump, _entity) { }

void JumpState::Init()
{
	StartAnimation();
	
	EntityCoordinator* ecs = GameData::Get().ecs;
	if (Physics* physics = ecs->GetComponent(Physics, entity))
	{
		physics->maxSpeed.y = 1000.0f;
		physics->speed.y = -300.0f;
	}
}

void JumpState::Update(float dt)
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
	Physics& physics = ecs->GetComponentRef(Physics, entity);

	const int jump_acceleration_factor = 2;
	physics.maxSpeed.x = 800.0f;
	physics.ApplyMovementEase(state.movementInput.toFloat(), dt, jump_acceleration_factor);

	// rapidly slow upwards movement while not holding space
	InputManager* input = InputManager::Get();
	if (!input->isHeld(Button::Space))
	{
		if (physics.speed.y < 0)
		{
			physics.speed.y -= physics.speed.y * 8.0f * dt;
		}
	}

	// moving downwards
	if (physics.speed.y >= 0.0f)
	{
		//ReplaceState(Fall);
		PopState();
	}
}

// JumpState
// ---------------------------------------------------------
FallState::FallState(ECS::Entity _entity) : CharacterAction(ActionState::Fall, _entity) { }

void FallState::Init()
{
	StartAnimation();
}

void FallState::Update(float dt)
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	Physics& physics = ecs->GetComponentRef(Physics, entity);
	if(physics.onFloor)
	{
		CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
		PopState();
	}
}

// RollState
// ---------------------------------------------------------
RollState::RollState(Entity _entity) : CharacterAction(ActionState::Roll, _entity) { }

void RollState::Init()
{
	EntityCoordinator* ecs = GameData::Get().ecs;

	StartAnimation(ActionState::Roll, false);

	if (Physics* physics = ecs->GetComponent(Physics,entity))
	{
		CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

		const float c_rollSpeed = 35.0f;
		physics->speed = state.movementInput.toFloat() * c_rollSpeed;
	}

	if(Collider* collider = ecs->GetComponent(Collider, entity))
	{
		collider->SetFlag(ECS::Collider::TerrainOnly);
	}
}

void RollState::Update(float dt)
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
	Physics& physics = ecs->GetComponentRef(Physics, entity);
	
	const int roll_acceleration_factor = 4;
	physics.maxSpeed.x = 35.0f;
	physics.ApplyMovementEase(state.movementInput.toFloat(), dt, roll_acceleration_factor);
	
	const Animator& animation = ecs->GetComponentRef(Animator, entity);
	if (animation.loopCount > 0)
	{
		CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
		state.actions.Pop();
		return;
	}
}

void RollState::Exit()
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	if(Collider* collider = ecs->GetComponent(Collider, entity))
	{
		collider->RemoveFlag(Collider::TerrainOnly);
	}
}


// BasicAttack
// ---------------------------------------------------------
BasicAttackState::BasicAttackState(ECS::Entity _entity) : CharacterAction(ActionState::BasicAttack, _entity) { }

void BasicAttackState::Init()
{
	StartAnimation(false);
	
	attackCollider = CreateNewAttackCollider("player attack collider", 10.0f, 30.0f);
	damageOnLoopCount = -1;
}

void BasicAttackState::Update(float dt)
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
	Animator& animator = ecs->GetComponentRef(Animator, entity);

	if (animator.loopCount > 0)
	{
		Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
		sprite.canFlip = true;

		if (animator.GetActiveAnimation().action == ActionState::BasicAttack)
		{
			InputManager* input = InputManager::Get();
			if (input->isCursorHeld(Cursor::ButtonType::Left))
			{
				StartAnimation(ActionState::BasicAttackHold);
				damageOnLoopCount = -1;
			}
			else
			{
				state.actions.Pop();
				return;
			}
		}
		else if (animator.GetActiveAnimation().action == ActionState::BasicAttackHold)
		{
			InputManager* input = InputManager::Get();
			if (!input->isCursorHeld(Cursor::ButtonType::Left))
			{
				state.actions.Pop();
				return;
			}
		}
	}
	
	if(animator.loopCount > damageOnLoopCount)
	{
		Damage& damage = ecs->GetComponentRef(Damage, attackCollider);
		damage.appliedTo.clear();
		damage.appliedTo.push_back(entity);

		damageOnLoopCount = animator.loopCount;
	}

	if (Physics* physics = ecs->GetComponent(Physics, entity))
	{
		physics->ApplyDrag(0.5f);
	}
}

void BasicAttackState::Exit()
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	ecs->entities.KillEntity(attackCollider);
}

// DeathState
// ---------------------------------------------------------
DeathState::DeathState(Entity _entity) : CharacterAction(ActionState::Death, _entity) { }

void DeathState::Init()
{
	can_respawn = false;

	StartAnimation(false);
	
	EntityCoordinator* ecs = GameData::Get().ecs;
	if(Physics* physics = ecs->GetComponent(Physics, entity))
	{
		physics->speed.set(0.0f, 0.0f);
	}
}

void DeathState::Update(float dt)
{	
	EntityCoordinator* ecs = GameData::Get().ecs;
	Animator& animation = ecs->GetComponentRef(Animator, entity);

	if(animation.loopCount > 0)
		can_respawn = true;
}

