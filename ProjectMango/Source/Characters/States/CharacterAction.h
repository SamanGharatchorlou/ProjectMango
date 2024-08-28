#pragma once

#include "Game/States/State.h"

enum class ActionState;

struct CharacterAction : public State
{
	CharacterAction();
	CharacterAction(ActionState _action, ECS::Entity _entity) : action(_action), entity(_entity) { }

	void StartAnimation();
	void StartAnimation(ActionState action);

	ECS::Entity entity;
	ActionState action;
};
