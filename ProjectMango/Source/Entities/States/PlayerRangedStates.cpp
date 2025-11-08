#include "pch.h"
#include "PlayerRangedStates.h"

#include "Animations/CharacterStates.h"
#include "Input/InputManager.h"

#include "ECS/Components/Animator.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/SpellComponents.h"
#include "ECS/Components/Physics.h"
#include "ECS/EntityCoordinator.h"
#include "Entities/Weapons/SpellEntityBuilder.h"
#include "ECS/Components/UIComponents.h"

#include "ECS/Components/GunComponents.h"
#include "Entities/Weapons/GunEntityBuilder.h"

using namespace PlayerRanged;
using namespace ECS;

static const int c_defaultEasingSpeed = 3;

static void UpdateShoot(ECS::Entity entity)
{
	InputManager* input = InputManager::Get();
	if (input->isCursorPressed(Cursor::ButtonType::Left, c_inputBuffer) || input->isCursorHeld(Cursor::ButtonType::Left))
	{
		if(Entity gun_entity = GetFirstChild(entity))
		{	 
			Firearm& firearm = GetComponentRef(Firearm, gun_entity);
			firearm.Fire();
		}
	}
}

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
	CharacterState& state = GetComponentRef(CharacterState, entity);

	// Run
	if(!state.movementInput.isZero())
	{
		PushState(Run);
		return;
	}
	else
	{
		Physics& physics = GetComponentRef(Physics, entity);
		physics.ApplyHorizontalDrag(0.4f);
	}

	// Jump
	InputManager* input = InputManager::Get();
	if (input->isPressed(Button::Space, c_inputBuffer))
	{
		PushState(Jump);
		return;
	}
	
	// Crouch
	if (input->isPressed(Button::Down))
	{
		PushState(Crouch);
		return;
	}

	UpdateShoot(entity);
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
	CharacterState& state = GetComponentRef(CharacterState, entity);
	Physics& physics = GetComponentRef(Physics, entity);
	
	if(state.movementInput.isZero())
	{
		PopState();
		return;
	}

	const int run_acceleration_factor = 3;
	physics.ApplyMovementEase(state.movementInput.toFloat(), dt, run_acceleration_factor);
	
	InputManager* input = InputManager::Get();
	if (input->isPressed(Button::Space, c_inputBuffer))
	{
		PushState(Jump);
		return;
	}

	if (input->isPressed(Button::Shift, c_inputBuffer))
	{
		PushState(Roll);
		return;
	}
	
	if (input->isPressed(Button::Down))
	{
		PushState(Crouch);
		return;
	}

	UpdateShoot(entity);
}

// JumpState
// ---------------------------------------------------------
JumpState::JumpState(Entity _entity) : CharacterAction(ActionState::Jump, _entity) { }

void JumpState::Init()
{
	StartAnimation();

	//Physics& physics = GetComponentRef(Physics, entity);
	//physics.speed.y = GetConfig(entity)->data.GetFloat("jump_impulse");

	cumulativeJump = 0.0f;
}

void JumpState::Update(float dt)
{
	CharacterState& state = GetComponentRef(CharacterState, entity);
	Physics& physics = GetComponentRef(Physics, entity);
	InputManager* input = InputManager::Get();

	if (input->isHeld(Button::Space))
	{
		if(cumulativeJump < GetConfig(entity)->data.GetFloat("jump_impulse"))
		{
			physics.speed.y += -200.0f;
			cumulativeJump += 200.0f;
			return;
		}
	}

	const int jump_acceleration_factor = 2;
	physics.ApplyMovementEase(state.movementInput.toFloat(), dt, jump_acceleration_factor);
	physics.ApplyHorizontalDrag(0.15f);

	// rapidly slow upwards movement while not holding space or slowing down
	if (!input->isHeld(Button::Space) || physics.speed.y > -200.0f)
	{
		if (physics.speed.y < 0)
		{
			physics.speed.y -= physics.speed.y * 25.0f * dt;
		}
	}

	// moving downwards
	if (physics.speed.y >= 0.0f)
	{
		PopState();
		return;
	}

	// debuff?
	UpdateShoot(entity);
}

// FallState
// ---------------------------------------------------------
FallState::FallState(ECS::Entity _entity) : CharacterAction(ActionState::Fall, _entity) { }

void FallState::Init()
{
	StartAnimation();

	Physics& physics = GetComponentRef(Physics, entity);
	if(physics.speed.y < 0.0f)
		physics.speed.y = 0.0f;
}
void FallState::Resume()
{
	StartAnimation();
}

void FallState::Update(float dt)
{
	CharacterState& state = GetComponentRef(CharacterState, entity);
	Physics& physics = GetComponentRef(Physics, entity);

	const int jump_acceleration_factor = 2;
	physics.ApplyMovementEase(state.movementInput.toFloat(), dt, jump_acceleration_factor);
	physics.ApplyHorizontalDrag(0.2f);

	if(physics.onFloor)
	{
		CharacterState& state = GetComponentRef(CharacterState, entity);
		PopState();
		return;
	}

	// debuff?
	UpdateShoot(entity);
}

// RollState
// ---------------------------------------------------------
RollState::RollState(Entity _entity) : CharacterAction(ActionState::Roll, _entity) { }

void RollState::Init()
{
	StartAnimation(ActionState::Roll, false);

	Physics& physics = GetComponentRef(Physics,entity);

	//const CharacterState& state = GetComponentRef(CharacterState, entity);
	physics.speed = GetFacingDirectionVector(entity).toFloat() * GetConfig(entity)->data.GetFloat("roll_impulse");

	if(Collider* collider = GetComponent(Collider, entity))
	{
		collider->SetFlag(Collider::TerrainOnly);
	}
}

void RollState::Update(float dt)
{
	const Animator& animation = GetComponentRef(Animator, entity);
	if (animation.loopCount > 0)
	{
		CharacterState& state = GetComponentRef(CharacterState, entity);
		PopState();
		return;
	}
}

void RollState::Exit()
{
	if(Collider* collider = GetComponent(Collider, entity))
	{
		collider->RemoveFlag(Collider::TerrainOnly);
	}
}


// CrouchState
// ---------------------------------------------------------
CrouchState::CrouchState(Entity _entity) : CharacterAction(ActionState::Crouch, _entity) { }

void CrouchState::Init()
{
	StartAnimation(ActionState::Crouch, true);
}

void CrouchState::Resume()
{
	rollColldown = 2.0f;
	StartAnimation(ActionState::Crouch, true);
}

void CrouchState::Update(float dt)
{
	InputManager* input = InputManager::Get();
	CharacterState& state = GetComponentRef(CharacterState, entity);
	Physics& physics = GetComponentRef(Physics, entity);

	physics.ApplyHorizontalDrag(0.8f);

	rollColldown-= dt;

	if(input->isPressed(Button::Shift, c_inputBuffer) && rollColldown < 0.0f)
	{
		PushState(Roll);
	}
	
	UpdateShoot(entity);
		
	const Animator& animation = GetComponentRef(Animator, entity);
	if (animation.loopCount > 0)
	{
		if (!input->isHeld(Button::Down))
		{
			PopState();
			return;
		}
	}
}


// DeathState
// ---------------------------------------------------------
DeathState::DeathState(Entity _entity) : CharacterAction(ActionState::Death, _entity) { }

void DeathState::Init()
{
	canRespawn = false;

	StartAnimation(false);
	
	if(Physics* physics = GetComponent(Physics, entity))
	{
		physics->speed.set(0.0f, 0.0f);
	}
}

void DeathState::Update(float dt)
{	
	Animator& animator = GetComponentRef(Animator, entity);

	if(animator.loopCount > 0)
	{
		if(!deathTimer.IsRunning())
			deathTimer.Start();

		if(deathTimer.GetSeconds() > 2.0f)
			canRespawn = true;
	}
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

void BasicAttackState::Update(float dt)
{
	Animator& animator = GetComponentRef(Animator, entity);
	if(animator.loopCount > 0) 
	{		
		CharacterState& state = GetComponentRef(CharacterState, entity);
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
	//
	//entities.KillEntity(attackCollider);
}