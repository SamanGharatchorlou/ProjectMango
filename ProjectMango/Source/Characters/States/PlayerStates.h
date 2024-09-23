#pragma once

#include "CharacterAction.h"
#include "Core/Timer.h"

namespace Player
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

	struct FloorSlamState : public CharacterAction
	{
		FloorSlamState(ECS::Entity _entity);

		void Init() override;
		void Update(float dt) override;
		void Exit() override;
		
		ECS::Entity attackCollider = ECS::EntityInvalid;
		bool slammingFloor = false;
	};

	struct HoverState : public CharacterAction
	{
		HoverState(ECS::Entity _entity);

		void Init() override;
		void Update(float dt) override;
		void Exit() override;
	};

	struct RollState : public CharacterAction
	{
		RollState(ECS::Entity _entity);

		void Init() override;
		void Update(float dt) override;
		void Exit() override;
	};

	struct BasicAttackState : public CharacterAction
	{
		BasicAttackState(ECS::Entity _entity);

		void Init() override;
		void Update(float dt) override;
		void Exit() override;
		
		ECS::Entity attackCollider = ECS::EntityInvalid;

		int damageOnLoopCount = -1;
	};

	struct LungeAttackState : public CharacterAction
	{
		LungeAttackState(ECS::Entity _entity);

		void Init() override;
		void Update(float dt) override;
		void Exit() override;
		
		ECS::Entity attackCollider = ECS::EntityInvalid;
	};

	struct DeathState : public CharacterAction
	{
		DeathState(ECS::Entity _entity);

		void Init() override;
		void Update(float dt) override;

		TimerF deathTimer;
		bool canRespawn = false;
	};
}