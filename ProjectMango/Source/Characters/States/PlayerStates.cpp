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

static const int c_defaultEasingSpeed = 3;

// Idle
// ---------------------------------------------------------
IdleState::IdleState(ECS::Entity _entity) : CharacterAction(ActionState::Idle, _entity) { }

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
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

	// Run
	if(!state.movementInput.isZero())
	{
		PushNewState(Run);
	}
	else
	{
		if (ECS::Physics* physics = ecs->GetComponent(Physics, entity))
		{
			physics->ApplyDrag(0.6f);
		}
	}

	// Basic Attack
	InputManager* input = InputManager::Get();
	if (input->isCursorPressed(Cursor::ButtonType::Left, c_inputBuffer) || input->isCursorHeld(Cursor::ButtonType::Left))
	{
		PushNewState(BasicAttack);
	}

	if (input->isPressed(Button::Space, c_inputBuffer))
	{
		PushNewState(Jump);
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
	
	if(state.movementInput.isZero())
	{
		state.actions.Pop();
		return;
	}

	const int run_acceleration_factor = 3;
	physics.maxSpeed.x = 15.0f;
	physics.ApplyMovementEase(state.movementInput.toFloat(), dt, run_acceleration_factor);
	
	InputManager* input = InputManager::Get();
	if (input->isPressed(Button::Space, c_inputBuffer))
	{
		PushNewState(Jump);
	}

	if (input->isPressed(Button::Shift, c_inputBuffer))
	{
		PushNewState(Roll);
	}

	if (input->isCursorPressed(Cursor::ButtonType::Left, c_inputBuffer) || input->isCursorHeld(Cursor::ButtonType::Left))
	{
		PushNewState(BasicAttack);
	}
}


// JumpState
// ---------------------------------------------------------
JumpState::JumpState(ECS::Entity _entity) : CharacterAction(ActionState::Jump, _entity) { }

void JumpState::Init()
{
	StartAnimation(ActionState::JumpMovingUp);
	
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		
	if (ECS::Physics* physics = ecs->GetComponent(Physics, entity))
	{
		ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

		// apply walk speed
		physics->maxSpeed.y = 100.0f;
		physics->speed.y = -30.0f;
	}
}

void JumpState::Update(float dt)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
	ECS::Physics& physics = ecs->GetComponentRef(Physics, entity);

	// apply walk speed
	const int jump_acceleration_factor = 2;
	physics.maxSpeed.x = 10.0f;
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
		ECS::Animator& animation = ecs->GetComponentRef(Animator, entity);
		if (animation.GetActiveAnimation().action == ActionState::JumpMovingUp)
		{
			StartAnimation(ActionState::JumpMovingDown);
		}

		if (physics.onFloor)
		{
			state.actions.Pop();
			return;
		}
	}
}

// RollState
// ---------------------------------------------------------
RollState::RollState(ECS::Entity _entity) : CharacterAction(ActionState::Roll, _entity) { }

void RollState::Init()
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;

	StartAnimation(ActionState::Roll);

	if (ECS::Physics* physics = ecs->GetComponent(Physics,entity))
	{
		ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

		const float c_rollSpeed = 35.0f;
		physics->speed = state.movementInput.toFloat() * c_rollSpeed;
	}

	if(ECS::Collider* collider = ecs->GetComponent(Collider, entity))
	{
		collider->SetFlag(ECS::Collider::TerrainOnly);
	}

	ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
	sprite.canFlip = false;
}

void RollState::Update(float dt)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
	ECS::Physics& physics = ecs->GetComponentRef(Physics, entity);
	
	const int roll_acceleration_factor = 4;
	physics.maxSpeed.x = 35.0f;
	physics.ApplyMovementEase(state.movementInput.toFloat(), dt, roll_acceleration_factor);
	
	const ECS::Animator& animation = ecs->GetComponentRef(Animator, entity);
	if (animation.loopCount > 0)
	{
		ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
		state.actions.Pop();
		return;
	}
}

void RollState::Exit()
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	if(ECS::Collider* collider = ecs->GetComponent(Collider, entity))
	{
		collider->RemoveFlag(ECS::Collider::TerrainOnly);
	}
}


// BasicAttack
// ---------------------------------------------------------
BasicAttackState::BasicAttackState(ECS::Entity _entity) : CharacterAction(ActionState::BasicAttack, _entity) { }

void BasicAttackState::Init()
{
	StartAnimation();
	
	CreateNewAttackCollider();
	damageOnLoopCount = -1;
	
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
	sprite.canFlip = false;
}

void BasicAttackState::Update(float dt)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
	ECS::Animator& animator = ecs->GetComponentRef(Animator, entity);

	if (animator.loopCount > 0)
	{
		ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
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
		ECS::Damage& damage = ecs->GetComponentRef(Damage, attackCollider);
		damage.appliedTo.clear();
		damage.appliedTo.push_back(entity);

		damageOnLoopCount = animator.loopCount;
	}

	if (ECS::Physics* physics = ecs->GetComponent(Physics, entity))
	{
		physics->ApplyDrag(0.5f);
	}
}

void BasicAttackState::Exit()
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ecs->entities.KillEntity(attackCollider);
			
	ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
	sprite.canFlip = true;
}

void BasicAttackState::CreateNewAttackCollider()
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ecs->entities.KillEntity(attackCollider);
	
	const ECS::Animator& animator = ecs->GetComponentRef(Animator, entity);
	const ECS::Animation& animation = animator.GetActiveAnimation();
	const ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);
	VectorF pos =  transform.size * animation.attackColliderPos;
	VectorF size = transform.size * animation.attackColliderSize;
	RectF collider_rect(pos, size);
	
	attackCollider = ecs->CreateEntity("player attack collider");
	ECS::EntityData::SetParent(attackCollider, entity);

	// Transform
	ECS::Transform& attack_transform = ecs->AddComponent(Transform, attackCollider);
	attack_transform.SetLocalPosition(pos);
	attack_transform.size = size;

	// Collider
	ECS::Collider& collider = ecs->AddComponent(Collider, attackCollider);
	collider.SetBaseRect(collider_rect);
	collider.SetFlag(ECS::Collider::IsDamage);

	// Damage
	ECS::Damage& damage = ecs->AddComponent(Damage, attackCollider);
	damage.value = 10;

	// dont damage our self
	damage.appliedTo.push_back(entity);
	
	ECS::CharacterState& character_state = ecs->GetComponentRef(CharacterState, entity);
	damage.force = character_state.GetFacingDirection().toFloat() * 30.0f;
}


// DeathState
// ---------------------------------------------------------
DeathState::DeathState(ECS::Entity _entity) : CharacterAction(ActionState::Death, _entity) { }

void DeathState::Init()
{
	can_respawn = false;

	StartAnimation();
	
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	if(ECS::Physics* physics = ecs->GetComponent(Physics, entity))
	{
		physics->speed.set(0.0f, 0.0f);
	}
}

void DeathState::Update(float dt)
{	
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Animator& animation = ecs->GetComponentRef(Animator, entity);

	if(animation.loopCount > 0)
		can_respawn = true;
}

