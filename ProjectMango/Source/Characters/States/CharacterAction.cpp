#include "pch.h"
#include "CharacterAction.h"

#include "Animations/CharacterStates.h"
#include "Core/Helpers.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "ECS/Components/AIController.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Biome.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/ComponentCommon.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Physics.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/EntSystems/AnimationSystem.h"
#include "ECS/EntSystems/CollisionSystem.h"
#include "ECS/EntSystems/TransformSystem.h"
#include "Game/FrameRateController.h"
#include "System/Files/ConfigManager.h"


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

	ECS::Entity attack_collider = ecs->CreateEntity(collider_name);
	ecs->AddComponent(Transform, attack_collider);
	ecs->AddComponent(Collider, attack_collider);
	ecs->AddComponent(Damage, attack_collider);
	ECS::EntityData::SetParent(attack_collider, entity);

	const ECS::Animator& animator = ecs->GetComponentRef(Animator, entity);
	const ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);
	const ECS::Animation& animation = animator.GetActiveAnimation();
	const VectorF pos = transform.size * animation.attackColliderPos;
	const VectorF size = transform.size * animation.attackColliderSize;

	// Transform
	ECS::Transform& attack_transform = ecs->GetComponentRef(Transform, attack_collider);
	attack_transform.size = size;
	attack_transform.SetLocalPosition(pos);
	attack_transform.ignoreOutOfBounds = true;

	// Collider
	ECS::Collider& collider = ecs->GetComponentRef(Collider, attack_collider);
	collider.SetBaseRect(RectF(pos, size));
	collider.SetFlag(ECS::Collider::IsDamage);
	collider.UpdateFromTransform();

	// Damage
	ECS::Damage& damage = ecs->GetComponentRef(Damage, attack_collider);
	damage.value = damage_value;
	damage.appliedTo.push_back(entity); // dont damage our self
	
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

			return distance;
		}
	}

	return -1.0f;
}

ECS::Entity Character::Create(const char* id, const char* config_id, VectorF spawn_pos)
{
	using namespace ECS;

	const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>(config_id);

	EntityCoordinator* ecs = GameData::Get().ecs;
	Entity entity = ecs->CreateEntity(id);
	ecs->AddComponent(Transform, entity);
	ecs->AddComponent(Physics, entity);
	ecs->AddComponent(Animator, entity);
	ecs->AddComponent(Sprite, entity);
	ecs->AddComponent(Collider, entity);
	ecs->AddComponent(Health, entity);
	ecs->AddComponent(AIController, entity);
	ecs->AddComponent(Pathing, entity);
	ecs->AddComponent(CharacterState, entity);

	// Transform
	Transform& transform = ecs->GetComponentRef(Transform, entity);
	transform.Init(config->values, spawn_pos);

	// MovementPhysics
	Physics& physics = ecs->GetComponentRef(Physics, entity);
	physics.Init(config->values);

	// Animator
	Animator& animation = ecs->GetComponentRef(Animator, entity);
	animation.Init(config->animation.c_str());

	// Sprite
	Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
	sprite.renderLayer = 4;
	
	// Collider
	Collider& collider = ecs->GetComponentRef(Collider, entity);
	collider.SetBaseRect(RectF(VectorF::zero(), transform.size));
	collider.SetFlag(Collider::IsEnemy);
	collider.UpdateFromTransform();

	CollisionSystem::FindValidPosition(entity);

	// Health
	Health& health = ecs->GetComponentRef(Health, entity);
	health.Init(config->values);
	health.invulnerable = false;

	return entity;
}


// Helpers
// ---------------------------------------------------------
bool CharacterAction::CanEnterHitState()
{		
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	if(const ECS::Collider* collider = ecs->GetComponent(Collider, entity))
	{
		const FrameRateController& frc = FrameRateController::Get();
		if(collider->lastHitFrame != -1 && (collider->lastHitFrame + 20) >= frc.FrameCount())
		{
			return true;
		}
	}

	return false;
}

bool CharacterAction::CanMoveToTarget()
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::AIController& ai_controller = ecs->GetComponentRef(AIController, entity);
	return ai_controller.moveToTarget && ecs->IsAlive(ai_controller.target);
}

bool CharacterAction::CoolingFromAttack()
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::AIController& ai_controller = ecs->GetComponentRef(AIController, entity);

	if( ai_controller.cooldownTimer.IsRunning() )
	{
		if(ai_controller.cooldownTimer.GetSeconds() < ai_controller.attackCooldownTime)
			return true;
		else
			ai_controller.cooldownTimer.Stop();
	}

	return false;
}

void CharacterAction::InitDeathState()
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;

	if(ECS::Collider* collider = ecs->GetComponent(Collider, entity))
	{
		SetFlag(collider->flags, (u32)ECS::Collider::IgnoreAll);
	}

	if(ECS::Physics* physics = ecs->GetComponent(Physics, entity))
	{
		physics->speed.set(0.0f, 0.0f);
	}
}

void CharacterAction::ApplyMovementEase(int movement_factor, float dt)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
	ECS::Physics& physics = ecs->GetComponentRef(Physics, entity);
	ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);

	const int run_acceleration_factor = 1;
	const VectorI facing_direction = state.GetFacingDirection();
	physics.ApplyMovementEase(facing_direction.toFloat(), dt, run_acceleration_factor);
}

bool CharacterAction::CanCreateAttackCollider(ECS::Entity attack_collider)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	const ECS::Animator& animator = ecs->GetComponentRef(Animator, entity);
	const ECS::Animation& animation = animator.GetActiveAnimation();
	if(animation.attackColliderFrameStart != -1)
	{
		return animator.frameIndex >= animation.attackColliderFrameStart && attack_collider == ECS::EntityInvalid;
	}

	return false;
}
bool CharacterAction::CanDestroyAttackCollider(ECS::Entity attack_collider)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	const ECS::Animator& animator = ecs->GetComponentRef(Animator, entity);
	const ECS::Animation& animation = animator.GetActiveAnimation();
	if(animation.attackColliderFrameEnd != -1)
	{
		return animator.frameIndex >= animation.attackColliderFrameEnd && attack_collider != ECS::EntityInvalid;
	}

	return false;
}