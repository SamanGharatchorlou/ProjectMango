#include "pch.h"
#include "CharacterAction.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Collider.h"
#include "Core/Helpers.h"
#include "ECS/Components/Animator.h"


void CharacterAction::StartAnimation()
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Animator& animation = ecs->GetComponentRef(Animator, entity);
	animation.StartAnimation(action);
}

ECS::Entity CreateAttackCollider(ECS::Entity entity, const RectF& collider_rect, float damage, const char* entity_name)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Entity attack_collider = ecs->CreateEntity(entity_name);

	// Transform
	ECS::Transform& attack_transform = ecs->AddComponent(Transform, attack_collider);
	//attack_transform.rect = collider_rect;
	//attack_transform.targetCenterPosition = collider_rect.Center(); 

	// Collider
	ECS::Collider& collider = ecs->AddComponent(Collider, attack_collider);
	collider.SetRect(collider_rect);
	collider.mFlags |= ECS::Collider::Flags::IgnoreAll;

	// Damage
	ECS::Damage& damage_comp = ecs->AddComponent(Damage, attack_collider);
	damage_comp.value = damage;
		
	// dont damage our self
	ECS::Health& health = ecs->GetComponentRef(Health, entity);
	health.ignoredDamaged.push_back(attack_collider);

	return attack_collider;
}

bool HandleAttackAnimation(ECS::Entity entity, ECS::Entity attack_collider)
{
	//ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	//ECS::Animation& animation = ecs->GetComponentRef(Animation, entity);
	//	
	//const Animation* anim = animation.animator.activeAnimation();
	//	
	//// activate the collider
	//if(animation.animator.mFrameIndex >= anim->colliderFrame)
	//{
	//	ECS::Collider& collider = ecs->GetComponentRef(Collider, attack_collider);
	//	RemoveFlag(collider.mFlags, (u32)ECS::Collider::IgnoreAll);
	//}

	//return animation.animator.finished();
	return false;
}