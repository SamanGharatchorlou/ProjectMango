#pragma once

#include "CharacterAction.h"

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

		void CreateNewAttackCollider();
		
		ECS::Entity attackCollider = ECS::EntityInvalid;

		u32 damageOnLoopCount = 0;
	};

	struct DeathState : public CharacterAction
	{
		DeathState(ECS::Entity _entity);

		void Init() override;
		void Update(float dt) override;

		bool can_respawn = false;
	};
}