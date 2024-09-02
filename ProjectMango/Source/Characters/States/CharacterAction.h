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

	ECS::Entity entity;
	ActionState action;
};
