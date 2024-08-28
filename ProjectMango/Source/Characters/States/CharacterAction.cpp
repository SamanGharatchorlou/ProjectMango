#include "pch.h"
#include "CharacterAction.h"

#include "Animations/CharacterStates.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Collider.h"
#include "Core/Helpers.h"
#include "ECS/Components/Animator.h"


CharacterAction::CharacterAction() : action(ActionState::None), entity(ECS::EntityInvalid)  { }

void CharacterAction::StartAnimation()
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Animator& animation = ecs->GetComponentRef(Animator, entity);
	animation.StartAnimation(action);

	// reset the sprite can flip state, default to true
	ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
	sprite.canFlip = true;
}

void CharacterAction::StartAnimation(ActionState action_state)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Animator& animation = ecs->GetComponentRef(Animator, entity);
	animation.StartAnimation(action_state);
}
