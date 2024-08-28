#pragma once

#include "Core/ObjectPool.h"
#include "Game/States/State.h"
#include "Animations/CharacterStates.h"
#include "CharacterAction.h"

namespace Player
{
	struct IdleState : public CharacterAction
	{
		IdleState(ECS::Entity _entity) : CharacterAction(ActionState::Idle, _entity) { }
		void Init() override;
		void Update(float dt) override;
		void Resume() override;
	};

	struct RunState : public CharacterAction
	{		
		RunState(ECS::Entity _entity) : CharacterAction(ActionState::Run, _entity) { }
		void Init() override;
		void Update(float dt) override;
		void Resume() override;
	};

	struct JumpState : public CharacterAction
	{
		JumpState(ECS::Entity _entity) : CharacterAction(ActionState::Jump, _entity) { }

		void Init() override;
		void Update(float dt) override;
	};

	struct RollState : public CharacterAction
	{
		RollState(ECS::Entity _entity) : CharacterAction(ActionState::Roll, _entity) { }

		void Init() override;
		void Update(float dt) override;
		void Exit() override;
	};

	struct BasicAttackState : public CharacterAction
	{
		BasicAttackState(ECS::Entity _entity) : CharacterAction(ActionState::BasicAttack, _entity) { }

		void Init() override;
		void Update(float dt) override;
		void Exit() override;
		
		ECS::Entity attackCollider = ECS::EntityInvalid;
	};

	//struct ChopAttackState : public CharacterAction
	//{
	//	void Update(float dt) override;
	//};

	struct DeathState : public CharacterAction
	{
		
		DeathState(ECS::Entity _entity) : CharacterAction(ActionState::Death, _entity) { }

		void Init() override;
		void Update(float dt) override;

		bool can_respawn = false;
	};
}