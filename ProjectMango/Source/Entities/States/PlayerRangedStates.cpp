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
#include "System/Files/ConfigManager.h"
#include "Entities/Spells/SpellEntityBuilder.h"
#include "ECS/Components/UIComponents.h"
//#include ""

using namespace PlayerRanged;
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
		return;
	}
	else
	{
		Physics& physics = ecs->GetComponentRef(Physics, entity);
		physics.ApplyDrag(0.4f);
	}

	// Jump
	InputManager* input = InputManager::Get();
	if (input->isPressed(Button::Space, c_inputBuffer))
	{
		PushState(Jump);
		return;
	}

	if (input->isCursorPressed(Cursor::ButtonType::Left, c_inputBuffer) || input->isCursorHeld(Cursor::ButtonType::Left))
	{
		PushState(BasicAttack);
		return;
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
}

// JumpState
// ---------------------------------------------------------
JumpState::JumpState(Entity _entity) : CharacterAction(ActionState::Jump, _entity) { }

void JumpState::Init()
{
	StartAnimation();
	
	EntityCoordinator* ecs = GameData::Get().ecs;
	Physics& physics = ecs->GetComponentRef(Physics, entity);
	physics.speed.y = GetObjectConfig(entity)->values.GetFloat("jump_impulse");
}

void JumpState::Update(float dt)
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
	Physics& physics = ecs->GetComponentRef(Physics, entity);

	const int jump_acceleration_factor = 2;
	physics.ApplyMovementEase(state.movementInput.toFloat(), dt, jump_acceleration_factor);
	physics.ApplyDrag(0.2f);

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
		PopState();
		return;
	}
}

// FallState
// ---------------------------------------------------------
FallState::FallState(ECS::Entity _entity) : CharacterAction(ActionState::Fall, _entity) { }

void FallState::Init()
{
	StartAnimation();
	
	EntityCoordinator* ecs = GameData::Get().ecs;
	Physics& physics = ecs->GetComponentRef(Physics, entity);
	if(physics.speed.y < 0.0f)
		physics.speed.y = 0.0f;
}
void FallState::Resume()
{
	StartAnimation();
}

void FallState::Update(float dt)
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
	Physics& physics = ecs->GetComponentRef(Physics, entity);

	const int jump_acceleration_factor = 2;
	physics.ApplyMovementEase(state.movementInput.toFloat(), dt, jump_acceleration_factor);
	physics.ApplyDrag(0.2f);

	if(physics.onFloor)
	{
		CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
		PopState();
		return;
	}
}

// RollState
// ---------------------------------------------------------
RollState::RollState(Entity _entity) : CharacterAction(ActionState::Roll, _entity) { }

void RollState::Init()
{
	StartAnimation(ActionState::Roll, false);

	EntityCoordinator* ecs = GameData::Get().ecs;
	Physics& physics = ecs->GetComponentRef(Physics,entity);

	const CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
	physics.speed = state.movementInput.toFloat() * GetObjectConfig(entity)->values.GetFloat("roll_impulse");

	if(Collider* collider = ecs->GetComponent(Collider, entity))
	{
		collider->SetFlag(Collider::TerrainOnly);
	}
}

void RollState::Update(float dt)
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	
	const Animator& animation = ecs->GetComponentRef(Animator, entity);
	if (animation.loopCount > 0)
	{
		CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
		PopState();
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



// DeathState
// ---------------------------------------------------------
DeathState::DeathState(Entity _entity) : CharacterAction(ActionState::Death, _entity) { }

void DeathState::Init()
{
	canRespawn = false;

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
	Animator& animator = ecs->GetComponentRef(Animator, entity);

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
	EntityCoordinator* ecs = GameData::Get().ecs;
	Animator& animator = ecs->GetComponentRef(Animator, entity);

	if(animator.GetAnimation(ActionState::AttackWindUp))
	{
		StartAnimation(ActionState::AttackWindUp);
	}
	else
	{	
		StartAnimation();
	}

	SpellBook& spell_book = ecs->GetComponentRef(SpellBook, entity);
	if(spell_book.CanActivateSpell(0))
		spell_book.ActivateSpellToCursor(0);
}

void BasicAttackState::Update(float dt)
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	Animator& animator = ecs->GetComponentRef(Animator, entity);
	if(animator.loopCount > 0)
	{		
		CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
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
	//EntityCoordinator* ecs = GameData::Get().ecs;
	//ecs->entities.KillEntity(attackCollider);
}