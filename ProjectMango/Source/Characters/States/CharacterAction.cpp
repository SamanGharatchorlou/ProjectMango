#include "pch.h"
#include "CharacterAction.h"

#include "Animations/CharacterStates.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Collider.h"
#include "Core/Helpers.h"
#include "ECS/Components/Animator.h"
#include "ECS/EntSystems/TransformSystem.h"


CharacterAction::CharacterAction() : action(ActionState::None), entity(ECS::EntityInvalid)  { }

void CharacterAction::StartAnimation(bool can_flip_sprite)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Animator& animation = ecs->GetComponentRef(Animator, entity);
	animation.StartAnimation(action);

	ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
	sprite.canFlip = can_flip_sprite;
}

void CharacterAction::StartAnimation(ActionState action_state, bool can_flip_sprite)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Animator& animation = ecs->GetComponentRef(Animator, entity);
	animation.StartAnimation(action_state);

	ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
	sprite.canFlip = can_flip_sprite;
}

ECS::Entity CharacterAction::CreateNewAttackCollider(const char* collider_name, float damage_value, float force_value)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;

	const ECS::Animator& animator = ecs->GetComponentRef(Animator, entity);
	const ECS::Animation& animation = animator.GetActiveAnimation();
	const ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);

	ECS::Entity attack_collider = ecs->CreateEntity(collider_name); 
	ECS::EntityData::SetParent(attack_collider, entity);

	const VectorF pos = transform.size * animation.attackColliderPos;
	const VectorF size = transform.size * animation.attackColliderSize;

	// Transform
	ECS::Transform& attack_transform = ecs->AddComponent(Transform, attack_collider);
	attack_transform.size = size;
	attack_transform.SetLocalPosition(pos);
	attack_transform.ignoreOutOfBounds = true;

	// Collider
	ECS::Collider& collider = ecs->AddComponent(Collider, attack_collider);
	collider.SetBaseRect(RectF(pos, size));
	collider.SetFlag(ECS::Collider::IsDamage);
	collider.SetPosFromTransform();

	// Damage
	ECS::Damage& damage = ecs->AddComponent(Damage, attack_collider);
	damage.value = damage_value;

	// dont damage our self
	damage.appliedTo.push_back(entity);
	
	ECS::CharacterState& character_state = ecs->GetComponentRef(CharacterState, entity);
	damage.force = force_value;
	damage.source = transform.GetObjectCenter();

	return attack_collider;
}

float CharacterAction::GetAttackRange(ActionState action)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;

	const ECS::Animator& animator = ecs->GetComponentRef(Animator, entity);
	if(const ECS::Animation* animation = animator.GetAnimation(action))
	{
		if(animation->attackColliderSize.x > 0.0f && animation->attackColliderSize.y > 0.0f)
		{
			const ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);

			const VectorF pos =  transform.worldPosition + transform.size * animation->attackColliderPos;
			const VectorF size = transform.size * animation->attackColliderSize;
			const RectF collider_rect(pos, size);

			const VectorF position = transform.GetObjectCenter();

			const float distance = std::max( std::abs(position.x - collider_rect.RightCenter().x), std::abs(position.x - collider_rect.LeftCenter().x) );

			return distance * 0.8f;
		}
	}

	return -1.0f;
}