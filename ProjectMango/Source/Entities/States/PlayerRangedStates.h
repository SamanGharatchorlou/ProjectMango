#pragma once

#include "CharacterAction.h"
#include "Core/Timer.h"

namespace PlayerRanged
{
	bool CanAttack(ECS::Entity entity);

	// todo: implement Character overloads and use that to init and die etc.

	struct IdleState : public CharacterAction
	{
		IdleState(ECS::Entity _entity);
		void Init() override;
		void Update(float dt) override;
		void Resume() override;

		float cooldown = 0.2f;
		float timer = 0.0f;
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

		float cumulativeJump = 0.0f;
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
		
	struct CrouchState : public CharacterAction
	{
		CrouchState(ECS::Entity _entity);

		void Init() override;
		void Update(float dt) override;

		void Resume() override;

		float rollColldown;
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
	};
}