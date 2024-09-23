#pragma once

#include "Game/States/State.h"
#include "Animations/CharacterStates.h"
#include "Characters/States/CharacterAction.h"

namespace ShockSweeper
{
	ECS::Entity Create(const char* id, const char* config_id, VectorF spawn_pos);

	struct Enemy : public Character
	{
		void Begin(ECS::Entity entity);
		bool FinishedDying(ECS::Entity entity) override;
		void StartDying(ECS::Entity entity) override;
	};

	struct IdleState : public CharacterAction
	{
		IdleState(ECS::Entity _entity) : CharacterAction(ActionState::Idle, _entity) { }
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

	struct TakeHitState : public CharacterAction
	{
		TakeHitState(ECS::Entity _entity) : CharacterAction(ActionState::TakeHit, _entity) { }
		void Init() override;
		void Update(float dt) override;
		void Exit() override;
	};

	struct DeathState : public CharacterAction
	{
		DeathState(ECS::Entity _entity) : CharacterAction(ActionState::Death, _entity) { }
		void Init() override;
		void Update(float dt) override;
		void Resume() override;
		
		bool can_kill = false;
	};
	
	struct BasicAttackState : public CharacterAction
	{
		BasicAttackState(ECS::Entity _entity);

		void Init() override;
		void Update(float dt) override;
		void Exit() override;
		
		ECS::Entity attackCollider = ECS::EntityInvalid;
	};
}