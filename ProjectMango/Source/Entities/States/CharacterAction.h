#pragma once

#include "Game/States/State.h"

enum class ActionState;

struct CharacterAction : public State
{
	CharacterAction();
	CharacterAction(ActionState _action, ECS::Entity _entity) : action(_action), entity(_entity) { }

	void StartAnimation(bool can_flip_sprite = true);
	void StartAnimation(ActionState action, bool can_flip_sprite = true);
	
	ECS::Entity CreateNewAttackCollider(const char* collider_name, float damage_value, float force_value);

	float GetAttackRange(ActionState action);

	// helpers
	bool CanEnterHitState();
	bool CanMoveToTarget();
	bool CoolingFromAttack();
	void InitDeathState();
	void ApplyMovementEase(int movement_factor, float dt);
	bool CanCreateAttackCollider(ECS::Entity attack_collider);
	bool CanDestroyAttackCollider(ECS::Entity attack_collider);

	ECS::Entity entity;
	ActionState action;
};

struct Character
{
	virtual void Begin(ECS::Entity entity) { }
	virtual bool FinishedDying(ECS::Entity entity) { return false; }
	virtual void StartDying(ECS::Entity entity) { }

	static ECS::Entity Create(const char* id, const char* config_id, VectorF spawn_pos);
};