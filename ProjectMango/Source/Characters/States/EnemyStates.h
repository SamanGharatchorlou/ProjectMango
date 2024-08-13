#pragma once

#include "Core/ObjectPool.h"
#include "Game/States/State.h"
#include "Animations/CharacterStates.h"
#include "CharacterAction.h"

namespace Enemy
{
	struct StatePool : public ObjectPool<CharacterAction, ActionState>
	{
		CharacterAction* createNewObjects(ActionState type, int count, int& outSize) const override;
	};

	struct IdleState : public CharacterAction
	{
		void Update(float dt) override;
	};

	struct RunState : public CharacterAction
	{
		void Update(float dt) override;
	};

	struct BasicAttackState : public CharacterAction
	{
		void Init() override;
		void Update(float dt) override;
		void Exit() override;

		ECS::Entity attackCollider = ECS::EntityInvalid;
	};

	struct DeathState : public CharacterAction
	{
		void Init() override;
		void Update(float dt) override;

		bool can_kill = false;
	};

	bool IsInAttackRange(ECS::Entity attacker, ECS::Entity target, VectorI& out_facing_direction);

	float GetAttackRange(ECS::Entity entity, const VectorI facing_direction);
	// same as attack range but same values in x and y for loops
	VectorF GetAttackRangeArea(ECS::Entity entity, const VectorI facing_direction);
	// this differs to attack range as it doesnt consider collider positioning, range is probably what you want not this one
	VectorF GetAttackArea(ECS::Entity entity, const VectorI facing_direction);
	RectF GetAttackRect(ECS::Entity entity, const VectorI facing_direction);
}