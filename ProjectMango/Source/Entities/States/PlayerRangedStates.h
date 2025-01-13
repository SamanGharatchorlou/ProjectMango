#pragma once

#include "CharacterAction.h"
#include "Core/Timer.h"

namespace PlayerRanged
{
	struct IdleState : public CharacterAction
	{
		IdleState(ECS::Entity _entity);
		void Init() override;
		void Update(float dt) override;
		void Resume() override;
	};

	struct RunState : public CharacterAction
	{		
		RunState(ECS::Entity _entity);
		void Init() override;
		void Update(float dt) override;
		void Resume() override;
	};

	struct JumpState : public CharacterAction
	{
		JumpState(ECS::Entity _entity);

		void Init() override;
		void Update(float dt) override;
	};

	struct FallState : public CharacterAction
	{
		FallState(ECS::Entity _entity);

		void Init() override;
		void Update(float dt) override;
		void Resume() override;
	};

	struct RollState : public CharacterAction
	{
		RollState(ECS::Entity _entity);

		void Init() override;
		void Update(float dt) override;
		void Exit() override;
	};

	struct DeathState : public CharacterAction
	{
		DeathState(ECS::Entity _entity);

		void Init() override;
		void Update(float dt) override;

		TimerF deathTimer;
		bool canRespawn = false;
	};

	struct BasicAttackState : CharacterAction
	{
		BasicAttackState(ECS::Entity _entity);

		void Init() override;
		void Update(float dt) override;
		void Exit() override;
		
		//bool
		//ECS::Entity attackCollider = ECS::EntityInvalid;
	};
}