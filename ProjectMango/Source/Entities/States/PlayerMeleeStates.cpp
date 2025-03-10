#include "pch.h"
#include "PlayerMeleeStates.h"

#include "Animations/CharacterStates.h"
#include "Input/InputManager.h"

#include "ECS/Components/Animator.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Physics.h"
#include "ECS/EntityCoordinator.h"
#include "System/Files/ConfigManager.h"

using namespace PlayerMelee;
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
		physics.ApplyDrag(0.4f);
	}

	// Basic Attack
	InputManager* input = InputManager::Get();
	if (input->isCursorPressed(Cursor::ButtonType::Left, c_inputBuffer) || input->isCursorHeld(Cursor::ButtonType::Left))
	{
		PushState(BasicAttack);
		return;
	}

	if (input->isPressed(Button::Space, c_inputBuffer))
	{
		PushState(Jump);
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

	if (input->isCursorPressed(Cursor::ButtonType::Left, c_inputBuffer) || input->isCursorHeld(Cursor::ButtonType::Left))
	{
		const Animator& animator = GetComponentRef(Animator, entity);
		const bool should_lunge = (float)animator.frameIndex >= (float)animator.GetActiveAnimation().frameCount * 0.25f;

		if (should_lunge)
		{
			ReplaceState(LungeAttack);
			return;
		}
		else
		{
			ReplaceState(BasicAttack);
			return;
		}
	}
}

// JumpState
// ---------------------------------------------------------
JumpState::JumpState(Entity _entity) : CharacterAction(ActionState::Jump, _entity) { }

void JumpState::Init()
{
	StartAnimation();
	
	Physics& physics = GetComponentRef(Physics, entity);
	physics.speed.y = GetConfig(entity)->data.GetFloat("jump_impulse");
}

void JumpState::Update(float dt)
{
	CharacterState& state = GetComponentRef(CharacterState, entity);
	Physics& physics = GetComponentRef(Physics, entity);

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

	if(input->isCursorPressed(Cursor::ButtonType::Right, c_inputBuffer))
	{
		if(state.canEnterHover)
		{
			ReplaceState(Hover);
			return;
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
	physics.ApplyDrag(0.2f);
	
	InputManager* input = InputManager::Get();
	if(input->isCursorPressed(Cursor::ButtonType::Right, c_inputBuffer))
	{
		if(state.canEnterHover)
		{
			PushState(Hover);
			return;
		}
	}

	if(physics.onFloor)
	{
		CharacterState& state = GetComponentRef(CharacterState, entity);
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

	Physics& physics = GetComponentRef(Physics,entity);

	const CharacterState& state = GetComponentRef(CharacterState, entity);
	physics.speed = state.movementInput.toFloat() * GetConfig(entity)->data.GetFloat("roll_impulse");

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


// -------------------------------------------------
// OLD PANDA STATES
// -------------------------------------------------

// BasicAttack
// ---------------------------------------------------------
BasicAttackState::BasicAttackState(ECS::Entity _entity) : CharacterAction(ActionState::BasicAttack, _entity) { }

void BasicAttackState::Init()
{
	StartAnimation(false);
}

void BasicAttackState::Update(float dt)
{
	
	CharacterState& state = GetComponentRef(CharacterState, entity);
	Animator& animator = GetComponentRef(Animator, entity);

	if (animator.loopCount > 0)
	{
		Sprite& sprite = GetComponentRef(Sprite, entity);
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
				PopState();
				return;
			}
		}
		else if (animator.GetActiveAnimation().action == ActionState::BasicAttackHold)
		{
			InputManager* input = InputManager::Get();
			if (!input->isCursorHeld(Cursor::ButtonType::Left))
			{
				PopState();
				return;
			}
		}
		else
		{
			PopState();
			return;
		}
	}
	
	if(animator.loopCount > damageOnLoopCount)
	{
		if(attackCollider == EntityInvalid)
		{					
			const CharacterState& state = GetComponentRef(CharacterState, entity);
			const Config* config = GetConfig(entity);
			attackCollider = CreateNewAttackCollider("player attack collider", config->data.GetFloat("basic_attack_damage"), config->data.GetFloat("basic_attack_force"));
		}

		Damage& damage = GetComponentRef(Damage, attackCollider);
		damage.appliedTo.clear();
		damage.appliedTo.push_back(entity);

		damageOnLoopCount = animator.loopCount;
	}

	Physics& physics = GetComponentRef(Physics, entity);
	physics.ApplyDrag(0.5f);
}

void BasicAttackState::Exit()
{
	ecs->entities.KillEntity(attackCollider);
}

// LungeAttack
// ---------------------------------------------------------
LungeAttackState::LungeAttackState(ECS::Entity _entity) : CharacterAction(ActionState::LungeAttack, _entity) { }

void LungeAttackState::Init()
{
	StartAnimation(false);	
}

void LungeAttackState::Update(float dt)
{
	

	Physics& physics = GetComponentRef(Physics, entity);
	physics.ApplyDrag(0.02f);

	const Animator& animation = GetComponentRef(Animator, entity);
	if (animation.loopCount > 0)
	{
		CharacterState& state = GetComponentRef(CharacterState, entity);
		PopState();
		return;
	}

	Animator& animator = GetComponentRef(Animator, entity);

	if(CanCreateAttackCollider(attackCollider))
	{					
		const CharacterState& state = GetComponentRef(CharacterState, entity);
		const Config* config = GetConfig(entity);
		attackCollider = CreateNewAttackCollider("player lunge attack collider", config->data.GetFloat("jump_attack_damage"), config->data.GetFloat("jump_attack_force"));
	}

	if(CanDestroyAttackCollider(attackCollider))
	{
		ecs->entities.KillEntity(attackCollider);
	}
}

void LungeAttackState::Exit()
{
	ecs->entities.KillEntity(attackCollider);
}

// HoverState
// ---------------------------------------------------------
HoverState::HoverState(ECS::Entity _entity) : CharacterAction(ActionState::Hover, _entity) { }

void HoverState::Init()
{
	StartAnimation();
	
	
	Physics& physics = GetComponentRef(Physics, entity);
	physics.applyGravity = false;
	physics.speed.y = 0.0f;
}

void HoverState::Update(float dt)
{
	
	
	CharacterState& state = GetComponentRef(CharacterState, entity);
	Physics& physics = GetComponentRef(Physics, entity);
	const int jump_acceleration_factor = 2;
	physics.ApplyMovementEase(state.movementInput.toFloat(), dt, jump_acceleration_factor);
	physics.ApplyDrag(0.3f);

	InputManager* input = InputManager::Get();
	if(input->isCursorPressed(Cursor::ButtonType::Left, c_inputBuffer))
	{
		ReplaceState(FloorSlam);
		return;
	}

	const Animator& animation = GetComponentRef(Animator, entity);
	if (animation.loopCount > 0)
	{
		PopState();
		return;
	}

}

void HoverState::Exit()
{
	
	Physics& physics = GetComponentRef(Physics, entity);
	physics.applyGravity = true;
}


// FloorSlamState
// ---------------------------------------------------------
FloorSlamState::FloorSlamState(ECS::Entity _entity) : CharacterAction(ActionState::FloorSlam, _entity) { }

void FloorSlamState::Init()
{
	StartAnimation(ActionState::Hover, false);
	
	
	if(Collider* collider = GetComponent(Collider, entity))
	{
		collider->SetFlag(Collider::TerrainOnly);
	}
}

void FloorSlamState::Update(float dt)
{
	
	Physics& physics = GetComponentRef(Physics, entity);

	// falling
	if( !physics.onFloor)
	{
		const int fall_acceleration_factor = 3;
		physics.ApplyMovementEase(VectorF(0,1.0f), dt, fall_acceleration_factor);
		physics.ApplyDrag(0.1f);
	}
	// slammin
	else
	{
		if(!slammingFloor)
		{
			StartAnimation(ActionState::FloorSlam, false);
			physics.speed = VectorF::zero();
			slammingFloor = true;
		}
		else
		{
			const Animator& animator = GetComponentRef(Animator, entity);
			if (animator.loopCount > 0)
			{
				CharacterState& state = GetComponentRef(CharacterState, entity);
				PopState();
				return;
			}

			if( CanCreateAttackCollider(attackCollider))
			{
				const Config* config = GetConfig(entity);
				attackCollider = CreateNewAttackCollider( "player slam attack collider", config->data.GetFloat("slam_attack_damage"), config->data.GetFloat("slam_attack_force") );
			}
			
			if( CanDestroyAttackCollider(attackCollider))
			{
				ecs->entities.KillEntity(attackCollider);
				attackCollider = EntityInvalid;
			}
		}
	}
}

void FloorSlamState::Exit()
{
	
	Physics& physics = GetComponentRef(Physics, entity);
	physics.applyGravity = true;

	if(Collider* collider = GetComponent(Collider, entity))
	{
		collider->RemoveFlag(Collider::TerrainOnly);
	}

	ecs->entities.KillEntity(attackCollider);
}