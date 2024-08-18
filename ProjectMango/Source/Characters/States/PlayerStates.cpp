#include "pch.h"
#include "PlayerStates.h"

#include "Input/InputManager.h"
#include "Animations/CharacterStates.h"

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
	//ECS::PlayerController& pc = ecs->GetComponentRef(PlayerController, entity);
	ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

	// Run
	if(!state.movementInput.isZero())
	{
		PushNewState(Run);
	}

	// Slash Attack
	InputManager* input = InputManager::Get();
	if (input->isCursorPressed(Cursor::ButtonType::Left))
	{
		//state.actions.Push( new BasicAttackState(entity));
	}
	//if (input->isCursorPressed(Cursor::ButtonType::Right, c_inputBuffer))
	//{
	//	pc.PushState(ActionState::ChopAttack);
	//}

	if (input->isPressed(Button::Space, c_inputBuffer))
	{
		PushNewState(Jump);
	}

	if(ECS::Physics* physics = ecs->GetComponent(Physics, entity))
	{
		physics->speed.x = 0.0f;
	}
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
	ECS::PlayerController& pc = ecs->GetComponentRef(PlayerController, entity);
	ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
	
	if(state.movementInput.isZero())
	{
		state.actions.Pop();
		return;
	}

	if (ECS::Physics* physics = ecs->GetComponent(Physics, entity))
	{
		ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

		// apply walk speed
		physics->maxSpeed.x = 20.0f;
		physics->ApplyMovement(state.movementInput.toFloat(), dt);
		//physics->ApplyDrag(state.movementInput.toFloat(), 0.9f);
	}
	
	InputManager* input = InputManager::Get();
	if (input->isPressed(Button::Space, c_inputBuffer))
	{
		PushNewState(Jump);
	}

	//// Slash Attack
	//InputManager* input = InputManager::Get();
	//if (input->isCursorPressed(Cursor::ButtonType::Left, c_inputBuffer))
	//{
	//	state.actions.Push( new BasicAttackState);
	//}
	//if (input->isCursorPressed(Cursor::ButtonType::Right, c_inputBuffer))
	//{
	//	pc.PushState(ActionState::ChopAttack);
	//}

	//// Dodge
	//if (input->isPressed(Button::Space, c_inputBuffer))
	//{
	//	pc.PushState(ActionState::Dodge);
	//}
}


// JumpState
// ---------------------------------------------------------
void JumpState::Init()
{
	StartAnimation();
	
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

	if (ECS::Physics* physics = ecs->GetComponent(Physics, entity))
	{
		ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

		// apply walk speed
		physics->maxSpeed.x = 20.0f;
		physics->ApplyMovement(state.movementInput.toFloat(), dt);
		//physics->ApplyMovement(state.movementInput.toFloat(), dt);
		//physics->ApplyDrag(state.movementInput.toFloat(), 0.9f);
	}

	if (ECS::Physics* physics = ecs->GetComponent(Physics, entity))
	{
		if(physics->onFloor && physics->speed.y >= 0.0f)
		{
			ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
			state.actions.Pop();
			return;
		}
	}
}

// DodgeState
// ---------------------------------------------------------
void DodgeState::Init()
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;

	if (ECS::Physics* physics = ecs->GetComponent(Physics,entity))
	{
		ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

		float amplitude = 10.0f;
		physics->speed = state.movementInput.toFloat() * amplitude;
	}

	if(ECS::Collider* collider = ecs->GetComponent(Collider, entity))
	{
		SetFlag(collider->mFlags, (u32)ECS::Collider::IgnoreAll);
	}
}

void DodgeState::Update(float dt)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;

	if (ECS::Physics* physics = ecs->GetComponent(Physics, entity))
	{
		physics->ApplyDrag(VectorF::zero(), 0.08f);
	}
	
	//ECS::Animation& animation = ecs->GetComponentRef(Animation, entity);
	//if(animation.animator.finished())
	//{
	//	ECS::PlayerController& pc = ecs->GetComponentRef(PlayerController, entity);
	//	pc.PopState();
	//}
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

void DodgeState::Exit()
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	if(ECS::Collider* collider = ecs->GetComponent(Collider, entity))
	{
		RemoveFlag(collider->mFlags, (u32)ECS::Collider::IgnoreAll);
		SetFlag(collider->mFlags, (u32)ECS::Collider::IgnoreCollisions);
	}
}


// SlashAttack
// ---------------------------------------------------------
void BasicAttackState::Init()
{
	//ECS::EntityCoordinator* ecs = GameData::Get().ecs;

	//const ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);
	//const ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

	//const RectF& character_rect = ECS::GetObjectRect(entity);
	//const RectF boundary_rect = ECS::GetRenderRect(entity);

	//RectF collider_rect;
	//	
	//if(!state.movementInput.isZero())
	//{
	//	if( direction.x == -1 || direction.x == 1 )
	//	{
	//		const float width = boundary_rect.Width() * 0.7f;
	//		const float height = boundary_rect.Height() * 0.3f;
	//		const VectorF size(width, height);
	//		collider_rect.SetSize(size);

	//		if( direction.x == 1 )
	//			collider_rect.SetTopLeft(character_rect.Center());
	//		else
	//			collider_rect.SetTopRight(character_rect.Center());
	//	}
	//	else if( direction.y == -1 || direction.y == 1 )
	//	{
	//		const float height = boundary_rect.Height() * 0.45f;
	//		const float width = boundary_rect.Width() * 0.65f;
	//		const VectorF size(width, height);
	//		collider_rect.SetSize(size);

	//		if( direction.y == 1 )
	//			collider_rect.SetTopCenter(character_rect.Center());
	//		else
	//			collider_rect.SetBotCenter(character_rect.Center());
	//	}
	//}

	//attackCollider = CreateAttackCollider(entity, collider_rect, 60, "Player Attack Collider");
}

void BasicAttackState::Update(float dt)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::PlayerController& pc = ecs->GetComponentRef(PlayerController, entity);
	ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
	
	if (ECS::Physics* physics = ecs->GetComponent(Physics, entity))
	{
		physics->ApplyDrag(VectorF::zero(), 0.2f);
	}
	
	if(HandleAttackAnimation(entity, attackCollider))
	{
		state.actions.Pop();
	}
		state.actions.Pop();
	
	//ECS::Animation& animation = ecs->GetComponentRef(Animation, entity);
	//if(animation.animator.lastFrame())
	//{
	//	// input buffer
	//	InputManager* input = InputManager::Get();
	//	if (input->isCursorPressed(Cursor::ButtonType::Left))
	//	{
	//		animation.animator.restart();

	//		pc.PopState();
	//		pc.PushState(ActionState::BasicAttack);
	//	}
	//}
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
