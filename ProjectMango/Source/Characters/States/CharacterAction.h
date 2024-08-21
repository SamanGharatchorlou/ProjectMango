#pragma once

//#include "ECS/EntityCommon.h"
//#include "Core/ObjectPool.h"
#include "Game/States/State.h"
#include "Animations/CharacterStates.h"

struct CharacterAction : public State
{
	CharacterAction() : action(ActionState::None), entity(ECS::EntityInvalid)  { }
	CharacterAction(ActionState _action, ECS::Entity _entity) : action(_action), entity(_entity) { }

	//void InitState(ECS::Entity entity_id, ActionState state) { entity = entity_id; action = state; }

	void StartAnimation();
	void StartAnimation(ActionState action);

	ECS::Entity entity;
	ActionState action;
};

ECS::Entity CreateAttackCollider(ECS::Entity entity, const RectF& rect, float damage, const char* entity_name);
bool HandleAttackAnimation(ECS::Entity entity, ECS::Entity attack_collider);