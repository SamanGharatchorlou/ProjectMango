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


using namespace ECS;

CharacterAction::CharacterAction() : action(ActionState::None), entity(EntityInvalid)  { }

void CharacterAction::StartAnimation(bool can_flip_sprite)
{
	Animator& animation = GetComponentRef(Animator, entity);
	animation.StartAnimation(action);

	Sprite& sprite = GetComponentRef(Sprite, entity);
	sprite.canFlip = can_flip_sprite;
}

void CharacterAction::StartAnimation(ActionState action_state, bool can_flip_sprite)
{
	Animator& animation = GetComponentRef(Animator, entity);
	animation.StartAnimation(action_state);

	Sprite& sprite = GetComponentRef(Sprite, entity);
	sprite.canFlip = can_flip_sprite;
}

Entity CharacterAction::CreateNewAttackCollider(const char* collider_name, float damage_value, float force_value)
{
	Entity attack_collider = ECS::CreateEntity(collider_name);
	AddComponent(Transform, attack_collider);
	AddComponent(Collider, attack_collider);
	AddComponent(Damage, attack_collider);
	EntityData::SetParent(attack_collider, entity);

	const Animator& animator = GetComponentRef(Animator, entity);
	const Transform& transform = GetComponentRef(Transform, entity);
	const Animation& animation = animator.GetActiveAnimation();
	const VectorF pos = transform.size * animation.attackColliderPos;
	const VectorF size = transform.size * animation.attackColliderSize;

	// Transform
	Transform& attack_transform = GetComponentRef(Transform, attack_collider);
	attack_transform.size = size;
	attack_transform.SetLocalPosition(pos);
	attack_transform.ignoreOutOfBounds = true;

	// Collider
	Collider& collider = GetComponentRef(Collider, attack_collider);
	attack_transform.InitCollider(collider);
	collider.SetFlag(Collider::IsDamage);

	// Damage
	Damage& damage = GetComponentRef(Damage, attack_collider);
	damage.value = damage_value;
	damage.appliedTo.push_back(entity); // dont damage our self
	
	CharacterState& character_state = GetComponentRef(CharacterState, entity);
	damage.force = force_value;
	damage.source = transform.GetObjectCenter();

	return attack_collider;
}

float CharacterAction::GetAttackRange(ActionState action)
{
	const Animator& animator = GetComponentRef(Animator, entity);
	if(const Animation* animation = animator.GetAnimation(action))
	{
		if(animation->attackColliderSize.x > 0.0f && animation->attackColliderSize.y > 0.0f)
		{
			const Transform& transform = GetComponentRef(Transform, entity);

			const VectorF pos =  transform.worldPosition + transform.size * animation->attackColliderPos;
			const VectorF size = transform.size * animation->attackColliderSize;
			const RectF collider_rect(pos, size);

			const VectorF position = transform.GetObjectCenter();

			const float distance = Maths::Max( std::abs(position.x - collider_rect.RightCenter().x), std::abs(position.x - collider_rect.LeftCenter().x) );

			return distance;
		}
	}

	return -1.0f;
}

Entity Character::CreateBasic(const ECS::EntityMetaData& emd)
{
	// adding everything something NEEDS to be an enemy... pretty much anyway
	Entity entity = ECS::CreateEntity(emd.id.c_str(), emd.ConfigId().c_str());
	AddComponent(Transform, entity);
	AddComponent(Physics, entity);
	AddComponent(Animator, entity);
	AddComponent(Sprite, entity);
	AddComponent(Collider, entity);
	AddComponent(Health, entity);

	const ObjectConfig* config = ECS::GetObjectConfig(entity);

	// Transform
	Transform& transform = GetComponentRef(Transform, entity);
	Collider& collider = GetComponentRef(Collider, entity);
	transform.Init(config, emd.position, collider);

	// MovementPhysics
	Physics& physics = GetComponentRef(Physics, entity);
	physics.Init(config);

	// Animator
	Animator& animation = GetComponentRef(Animator, entity);
	animation.Init(config);

	// Health
	Health& health = GetComponentRef(Health, entity);
	health.Init(config);

	// set sprite layer - default 5
	Sprite& sprite = GetComponentRef(Sprite, entity);
	sprite.Init(config);
	sprite.renderLayer = 5;

	// handle any tags here!
	// if (config->strings.Contains("tags")) etc

	return entity;
}

Entity Character::CreateBasicEnemy(const ECS::EntityMetaData& emd)
{
	Entity entity = Character::CreateBasic(emd);

	// make it an enemy
	AddComponent(AIController, entity);

	//todo: remove this, adding it into the function calling this any way
	// CharacterState
	CharacterState& character_state = AddComponent(CharacterState, entity);
	// this is specific to the enemy type and needs to be set in there
	character_state.character = nullptr;

	// set collider flags
	Collider& collider = GetComponentRef(Collider, entity);
	collider.SetFlag(Collider::IsEnemy);

	CollisionSystem::FindValidPosition(entity);

	return entity;
}


// Helpers
// ---------------------------------------------------------
bool CharacterAction::CanEnterHitState(float frame_buffer)
{		
	if(const Collider* collider = GetComponent(Collider, entity))
	{
		const FrameRateController& frc = FrameRateController::Get();
		if(collider->lastHitFrame != -1 && (collider->lastHitFrame + frame_buffer) >= frc.FrameCount())
		{
			return true;
		}
	}

	return false;
}

bool CharacterAction::CanMoveToTarget()
{
	AIController& ai_controller = GetComponentRef(AIController, entity);
	return ai_controller.canMoveToTarget && ecs->IsAlive(ai_controller.target);
}

bool CharacterAction::CoolingFromAttack()
{
	AIController& ai_controller = GetComponentRef(AIController, entity);

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
	if(Collider* collider = GetComponent(Collider, entity))
	{
		SetFlag(collider->flags, (u32)Collider::IgnoreAll);
	}

	if(Physics* physics = GetComponent(Physics, entity))
	{
		physics->speed.set(0.0f, 0.0f);
	}
}

void CharacterAction::ApplyMovementEase(int movement_factor, float dt)
{
	CharacterState& state = GetComponentRef(CharacterState, entity);
	Physics& physics = GetComponentRef(Physics, entity);

	const int run_acceleration_factor = 1;
	const VectorI facing_direction = GetFacingDirectionVector(entity);
	physics.ApplyMovementEase(facing_direction.toFloat(), dt, run_acceleration_factor);
}

bool CharacterAction::CanCreateAttackCollider(Entity attack_collider)
{
	const Animator& animator = GetComponentRef(Animator, entity);
	const Animation& animation = animator.GetActiveAnimation();
	if(animation.attackColliderFrameStart != -1)
	{
		return animator.frameIndex >= animation.attackColliderFrameStart && attack_collider == EntityInvalid;
	}

	return false;
}
bool CharacterAction::CanDestroyAttackCollider(Entity attack_collider)
{
	const Animator& animator = GetComponentRef(Animator, entity);
	const Animation& animation = animator.GetActiveAnimation();
	if(animation.attackColliderFrameEnd != -1)
	{
		return animator.frameIndex >= animation.attackColliderFrameEnd && attack_collider != EntityInvalid;
	}

	return false;
}