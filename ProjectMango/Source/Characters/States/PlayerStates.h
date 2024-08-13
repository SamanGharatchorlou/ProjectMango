#pragma once

#include "Core/ObjectPool.h"
#include "Game/States/State.h"
#include "Animations/CharacterStates.h"
#include "CharacterAction.h"

namespace Player
{
	struct StatePool : public ObjectPool<CharacterAction, ActionState>
	{
		CharacterAction* createNewObjects(ActionState type, int count, int& outSize) const override;
	};

	struct IdleState : public CharacterAction
	{
		void Update(float dt) override;
	};

	struct WalkState : public CharacterAction
	{
		void Update(float dt) override;
	};

	struct RunState : public CharacterAction
	{
		void Update(float dt) override;
	};

	struct DodgeState : public CharacterAction
	{
		void Init() override;
		void Update(float dt) override;
		void Exit() override;
	};

	struct BasicAttackState : public CharacterAction
	{
		void Init() override;
		void Update(float dt) override;
		void Exit() override;
		
		ECS::Entity attackCollider = ECS::EntityInvalid;
	};

	struct ChopAttackState : public CharacterAction
	{
		void Update(float dt) override;
	};

	struct DeathState : public CharacterAction
	{
		void Init() override;
		void Update(float dt) override;

		bool can_respawn = false;
	};
}