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
#include "System/Files/ConfigManager.h"

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
		return;
	}
	else
	{
		Physics& physics = ecs->GetComponentRef(Physics, entity);
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

	if (input->isCursorPressed(Cursor::ButtonType::Left, c_inputBuffer) || input->isCursorHeld(Cursor::ButtonType::Left))
	{
		const Animator& animator = ecs->GetComponentRef(Animator, entity);
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
	
	EntityCoordinator* ecs = GameData::Get().ecs;
	Physics& physics = ecs->GetComponentRef(Physics, entity);
	
	const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>("PlayerDataConfig");
	physics.speed.y = config->values.at("jump_impulse");
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

	if(input->isCursorPressed(Cursor::ButtonType::Right, c_inputBuffer))
	{
		PlayerController& pc = ecs->GetComponentRef(PlayerController, entity);
		if(pc.canEnterHover)
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

// JumpState
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
	
	InputManager* input = InputManager::Get();
	if(input->isCursorPressed(Cursor::ButtonType::Right, c_inputBuffer))
	{
		PlayerController& pc = ecs->GetComponentRef(PlayerController, entity);
		if(pc.canEnterHover)
		{
			PushState(Hover);
			return;
		}
	}

	if(physics.onFloor)
	{
		CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
		PopState();
		return;
	}
}

// HoverState
// ---------------------------------------------------------
HoverState::HoverState(ECS::Entity _entity) : CharacterAction(ActionState::Hover, _entity) { }

void HoverState::Init()
{
	StartAnimation();
	
	EntityCoordinator* ecs = GameData::Get().ecs;
	Physics& physics = ecs->GetComponentRef(Physics, entity);
	physics.applyGravity = false;
	physics.speed.y = 0.0f;

	PlayerController& pc = ecs->GetComponentRef(PlayerController, entity);
	//pc.canEnterHover = false;
}

void HoverState::Update(float dt)
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	
	CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
	Physics& physics = ecs->GetComponentRef(Physics, entity);
	const int jump_acceleration_factor = 2;
	physics.ApplyMovementEase(state.movementInput.toFloat(), dt, jump_acceleration_factor);
	physics.ApplyDrag(0.3f);

	InputManager* input = InputManager::Get();
	if(input->isCursorPressed(Cursor::ButtonType::Left, c_inputBuffer))
	{
		ReplaceState(FloorSlam);
		return;
	}

	const Animator& animation = ecs->GetComponentRef(Animator, entity);
	if (animation.loopCount > 0)
	{
		PopState();
		return;
	}

}

void HoverState::Exit()
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	Physics& physics = ecs->GetComponentRef(Physics, entity);
	physics.applyGravity = true;
}


// FloorSlamState
// ---------------------------------------------------------
FloorSlamState::FloorSlamState(ECS::Entity _entity) : CharacterAction(ActionState::FloorSlam, _entity) { }

void FloorSlamState::Init()
{
	StartAnimation(ActionState::Hover, false);
	
	EntityCoordinator* ecs = GameData::Get().ecs;
	if(Collider* collider = ecs->GetComponent(Collider, entity))
	{
		collider->SetFlag(Collider::TerrainOnly);
	}
}

void FloorSlamState::Update(float dt)
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	Physics& physics = ecs->GetComponentRef(Physics, entity);

	// falling
	if( !physics.onFloor)
	{
		const CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

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
			const Animator& animation = ecs->GetComponentRef(Animator, entity);
			if (animation.loopCount > 0)
			{
				CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
				PopState();
				return;
			}

			if(animation.frameIndex == 2 && attackCollider == EntityInvalid)
			{
				const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>("PlayerDataConfig");
				attackCollider = CreateNewAttackCollider("player slam attack collider", config->values.at("slam_attack_damage"), config->values.at("slam_attack_force"));
			}

			if(animation.frameIndex >= 5 && attackCollider != EntityInvalid)
			{
				ecs->entities.KillEntity(attackCollider);
				attackCollider = EntityInvalid;
			}
		}
	}
}

void FloorSlamState::Exit()
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	Physics& physics = ecs->GetComponentRef(Physics, entity);
	physics.applyGravity = true;

	if(Collider* collider = ecs->GetComponent(Collider, entity))
	{
		collider->RemoveFlag(Collider::TerrainOnly);
	}

	ecs->entities.KillEntity(attackCollider);
}

// RollState
// ---------------------------------------------------------
RollState::RollState(Entity _entity) : CharacterAction(ActionState::Roll, _entity) { }

void RollState::Init()
{
	StartAnimation(ActionState::Roll, false);

	EntityCoordinator* ecs = GameData::Get().ecs;
	Physics& physics = ecs->GetComponentRef(Physics,entity);
	CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
		
	const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>("PlayerDataConfig");
	physics.speed = state.movementInput.toFloat() * config->values.at("roll_impulse");

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

// BasicAttack
// ---------------------------------------------------------
BasicAttackState::BasicAttackState(ECS::Entity _entity) : CharacterAction(ActionState::BasicAttack, _entity) { }

void BasicAttackState::Init()
{
	StartAnimation(false);
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
			const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>("PlayerDataConfig");
			attackCollider = CreateNewAttackCollider("player attack collider", config->values.at("basic_attack_damage"), config->values.at("basic_attack_force"));
		}

		Damage& damage = ecs->GetComponentRef(Damage, attackCollider);
		damage.appliedTo.clear();
		damage.appliedTo.push_back(entity);

		damageOnLoopCount = animator.loopCount;
	}

	Physics& physics = ecs->GetComponentRef(Physics, entity);
	physics.ApplyDrag(0.5f);
}

void BasicAttackState::Exit()
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	//ecs->entities.KillEntity(attackCollider);
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


// LungeAttack
// ---------------------------------------------------------
LungeAttackState::LungeAttackState(ECS::Entity _entity) : CharacterAction(ActionState::LungeAttack, _entity) { }

void LungeAttackState::Init()
{
	StartAnimation(false);	
}

void LungeAttackState::Update(float dt)
{
	EntityCoordinator* ecs = GameData::Get().ecs;

	Physics& physics = ecs->GetComponentRef(Physics, entity);
	physics.ApplyDrag(0.02f);

	const Animator& animation = ecs->GetComponentRef(Animator, entity);
	if (animation.loopCount > 0)
	{
		CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
		PopState();
		return;
	}

	Animator& animator = ecs->GetComponentRef(Animator, entity);
	if(attackCollider == EntityInvalid && animation.frameIndex >= 9)
	{
		const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>("PlayerDataConfig");
		attackCollider = CreateNewAttackCollider("player lunge attack collider", config->values.at("jump_attack_damage"), config->values.at("jump_attack_force"));
	}

	if(attackCollider != EntityInvalid && animation.frameIndex >= 11)
	{
		ecs->entities.KillEntity(attackCollider);
	}
}

void LungeAttackState::Exit()
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	ecs->entities.KillEntity(attackCollider);
}