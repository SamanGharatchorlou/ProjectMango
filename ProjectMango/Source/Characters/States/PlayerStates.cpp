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
	else
	{
		const float speed = physics.speed.x;
		const int input_dir = state.movementInput.x;
		if (speed > 0.0f && input_dir < 0 || speed < 0.0f && input_dir > 0)
		{
			physics.ApplyDrag(0.9f);
		}
	}

	physics.maxSpeed.x = 15.0f;
	physics.ApplyMovement(state.movementInput.toFloat(), dt);
	
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
	physics.maxSpeed.x = 10.0f;
	physics.ApplyMovement(state.movementInput.toFloat(), dt);

	const float speed = physics.speed.x;
	const int input_dir = state.movementInput.x;
	if (speed > 0.0f && input_dir < 0 || speed < 0.0f && input_dir > 0)
	{
		physics.ApplyDrag(0.7f);
	}

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

	InputManager* input = InputManager::Get();
	if (input->isReleased(Button::Shift, c_inputBuffer))
	{
		const FrameRateController& frc = FrameRateController::Get();
		const int frame_count = frc.frameCount();

		const int release_frame = input->getButton(Button::Shift).mReleasedFrame;
		if (release_frame + c_inputBuffer < frame_count)
		{
			ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
			state.actions.Pop();
			return;
		}
	}

	//if(ECS::Collider* collider = ecs->GetComponent(Collider, entity))
	//{
	//	SetFlag(collider->flags, (u32)ECS::Collider::IgnoreAll);
	//}
}

void RollState::Update(float dt)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;

	//if (ECS::Physics* physics = ecs->GetComponent(Physics, entity))
	//{
	//	physics->ApplyDrag(0.08f);
	//}
	
	const ECS::Animator& animation = ecs->GetComponentRef(Animator, entity);
	if (animation.loopCount > 0)
	{
		ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
		state.actions.Pop();
	}
}

//if(ECS::Collider* collider = ecs->GetComponent(Collider, entity))
//{
//	if(HasFlag(collider->mFlags, Collider::IgnoreCollisions))
//	{
//		// remove the flag so we can make a "would it collide" check
//		// if so, add it back we'll end up in a bad state
//		RemoveFlag(collider->mFlags, (u32)Collider::IgnoreCollisions);
//		if(CollisionSystem::DoesColliderInteract(entity))
//		{
//			SetFlag(collider->mFlags, (u32)Collider::IgnoreCollisions);
//		}
//	}
//}

void RollState::Exit()
{
	//ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	//if(ECS::Collider* collider = ecs->GetComponent(Collider, entity))
	//{
	//	RemoveFlag(collider->flags, (u32)ECS::Collider::IgnoreAll);
	//	SetFlag(collider->flags, (u32)ECS::Collider::IgnoreCollisions);
	//}
}


// BasicAttack
// ---------------------------------------------------------
BasicAttackState::BasicAttackState(ECS::Entity _entity) : CharacterAction(ActionState::BasicAttack, _entity) { }

void BasicAttackState::Init()
{
	StartAnimation();
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
		CreateNewAttackCollider();
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
	//collider.flags |= ECS::Collider::Flags::IgnoreAll;

	// Damage
	ECS::Damage& damage = ecs->AddComponent(Damage, attackCollider);
	damage.value = 10;
	
	ECS::CharacterState& character_state = ecs->GetComponentRef(CharacterState, entity);
	damage.force = character_state.GetFacingDirection().toFloat() * 3.0f;
		
	// dont damage our self
	ECS::Health& health = ecs->GetComponentRef(Health, entity);
	health.ignoredDamaged.push_back(attackCollider);
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

