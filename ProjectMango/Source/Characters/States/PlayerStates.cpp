#include "pch.h"
#include "PlayerStates.h"

#include "Input/InputManager.h"
#include "Animations/CharacterStates.h"
#include "Game/FrameRateController.h"

#include "ECS/Components/PlayerController.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/EntSystems/AnimationSystem.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/ComponentCommon.h"
#include "Core/Helpers.h"

using namespace Player;

#define PushNewState(action) state.actions.Push(new action##State(entity))

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

	//if(ECS::Physics* physics = ecs->GetComponent(Physics, entity))
	//{
	//	physics->speed.x = 0.0f;
	//}
}

// Run
// ---------------------------------------------------------
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

	physics.maxSpeed.x = 20.0f;
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
	physics.maxSpeed.x = 15.0f;
	physics.ApplyMovement(state.movementInput.toFloat(), dt);

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
void BasicAttackState::Init()
{
	StartAnimation();

	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	const ECS::Animator& animator = ecs->GetComponentRef(Animator, entity);
	const ECS::Animation& animation = animator.GetActiveAnimation();

	const ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);
	RectF base_rect(transform.position, transform.size);
	
	VectorF pos = base_rect.TopLeft() + (base_rect.Size() * animation.attackColliderPos);
	VectorF size = base_rect.Size() * animation.attackColliderSize;

	attackCollider = CreateAttackCollider(entity, RectF(pos, size), 10, "player attack collider");
}

void BasicAttackState::Update(float dt)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
	ECS::Animator& animator = ecs->GetComponentRef(Animator, entity);
	if (animator.loopCount > 0)
	{
		if (animator.GetActiveAnimation().action == ActionState::BasicAttack)
		{
			InputManager* input = InputManager::Get();
			if (input->isCursorHeld(Cursor::ButtonType::Left))
			{
				StartAnimation(ActionState::BasicAttackHold);
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

	if (ECS::Physics* physics = ecs->GetComponent(Physics, entity))
	{
		physics->ApplyDrag(0.15f);
	}
}

void BasicAttackState::Exit()
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	GameData::Get().ecs->entities.KillEntity(attackCollider);
}


//// ChopAttack
//// ---------------------------------------------------------
//void ChopAttackState::Update(float dt)
//{
//	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
//	ECS::PlayerController& pc = ecs->GetComponentRef(PlayerController, entity);
//	
//	if (ECS::Physics* physics = ecs->GetComponent(Physics, entity))
//	{
//		physics->ApplyDrag(VectorF::zero(), 0.05f);
//	}
//	
//	//ECS::Animation& animation = ecs->GetComponentRef(Animation, entity);
//	//if(animation.animator.finished())
//	//{
//	//	pc.PopState();
//	//}
//
//	//if(animation.animator.lastFrame())
//	//{
//	//	// input buffer
//	//	InputManager* input = InputManager::Get();
//	//	if (input->isCursorPressed(Cursor::ButtonType::Left))
//	//	{
//	//		animation.animator.restart();			
//	//		
//	//		pc.PopState();
//	//		pc.PushState(ActionState::ChopAttack);;
//	//	}
//	//}
//}


// DeathState
// ---------------------------------------------------------
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

	//if(animation.animator.finished())
	//{
	//	can_respawn = true;
	//}
}
