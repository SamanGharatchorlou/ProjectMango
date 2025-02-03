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
	EntityCoordinator* ecs = GameData::Get().ecs;
	Animator& animation = ecs->GetComponentRef(Animator, entity);
	animation.StartAnimation(action);

	Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
	sprite.canFlip = can_flip_sprite;
}

void CharacterAction::StartAnimation(ActionState action_state, bool can_flip_sprite)
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	Animator& animation = ecs->GetComponentRef(Animator, entity);
	animation.StartAnimation(action_state);

	Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
	sprite.canFlip = can_flip_sprite;
}

Entity CharacterAction::CreateNewAttackCollider(const char* collider_name, float damage_value, float force_value)
{
	EntityCoordinator* ecs = GameData::Get().ecs;

	Entity attack_collider = ecs->CreateEntity(collider_name);
	ecs->AddComponent(Transform, attack_collider);
	ecs->AddComponent(Collider, attack_collider);
	ecs->AddComponent(Damage, attack_collider);
	EntityData::SetParent(attack_collider, entity);

	const Animator& animator = ecs->GetComponentRef(Animator, entity);
	const Transform& transform = ecs->GetComponentRef(Transform, entity);
	const Animation& animation = animator.GetActiveAnimation();
	const VectorF pos = transform.size * animation.attackColliderPos;
	const VectorF size = transform.size * animation.attackColliderSize;

	// Transform
	Transform& attack_transform = ecs->GetComponentRef(Transform, attack_collider);
	attack_transform.size = size;
	attack_transform.SetLocalPosition(pos);
	attack_transform.ignoreOutOfBounds = true;

	// Collider
	Collider& collider = ecs->GetComponentRef(Collider, attack_collider);
	attack_transform.InitCollider(collider);
	collider.SetFlag(Collider::IsDamage);

	// Damage
	Damage& damage = ecs->GetComponentRef(Damage, attack_collider);
	damage.value = damage_value;
	damage.appliedTo.push_back(entity); // dont damage our self
	
	CharacterState& character_state = ecs->GetComponentRef(CharacterState, entity);
	damage.force = force_value;
	damage.source = transform.GetObjectCenter();

	return attack_collider;
}

float CharacterAction::GetAttackRange(ActionState action)
{
	EntityCoordinator* ecs = GameData::Get().ecs;

	const Animator& animator = ecs->GetComponentRef(Animator, entity);
	if(const Animation* animation = animator.GetAnimation(action))
	{
		if(animation->attackColliderSize.x > 0.0f && animation->attackColliderSize.y > 0.0f)
		{
			const Transform& transform = ecs->GetComponentRef(Transform, entity);

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

Entity Character::CreateBasic(const char* id, const char* config_id, VectorF spawn_pos)
{
	const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>(config_id);

	// adding everything something NEEDS to be an enemy... pretty much anyway
	EntityCoordinator* ecs = GameData::Get().ecs;
	Entity entity = ecs->CreateEntity(id);
	ecs->AddComponent(Transform, entity);
	ecs->AddComponent(Physics, entity);
	ecs->AddComponent(Animator, entity);
	ecs->AddComponent(Sprite, entity);
	ecs->AddComponent(Collider, entity);
	ecs->AddComponent(Health, entity);

	// Transform
	Transform& transform = ecs->GetComponentRef(Transform, entity);
	Collider& collider = ecs->GetComponentRef(Collider, entity);
	transform.Init(config, spawn_pos, collider);

	// MovementPhysics
	Physics& physics = ecs->GetComponentRef(Physics, entity);
	physics.Init(config);

	// Animator
	Animator& animation = ecs->GetComponentRef(Animator, entity);
	animation.Init(config);

	// Health
	Health& health = ecs->GetComponentRef(Health, entity);
	health.Init(config);

	// set sprite layer - default 5
	Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
	sprite.Init(config);
	sprite.renderLayer = 5;

	// handle any tags here!
	// if (config->strings.Contains("tags")) etc

	return entity;
}

Entity Character::CreateBasicEnemy(const char* id, const char* config_id, VectorF spawn_pos)
{
	Entity entity = Character::CreateBasic(id, config_id, spawn_pos);

	const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>(config_id);
			
	// make it an enemy
	EntityCoordinator* ecs = GameData::Get().ecs;
	ecs->AddComponent(AIController, entity);

	// CharacterState
	CharacterState& character_state = ecs->AddComponent(CharacterState, entity);
	character_state.config = config_id;
	// this is specific to the enemy type and needs to be set in there
	character_state.character = nullptr;

	// set collider flags
	Collider& collider = ecs->GetComponentRef(Collider, entity);
	collider.SetFlag(Collider::IsEnemy);

	CollisionSystem::FindValidPosition(entity);
		
	return entity;
}


// Helpers
// ---------------------------------------------------------
bool CharacterAction::CanEnterHitState(float frame_buffer)
{		
	EntityCoordinator* ecs = GameData::Get().ecs;
	if(const Collider* collider = ecs->GetComponent(Collider, entity))
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
	EntityCoordinator* ecs = GameData::Get().ecs;
	AIController& ai_controller = ecs->GetComponentRef(AIController, entity);
	return ai_controller.moveToTarget && ecs->IsAlive(ai_controller.target);
}

bool CharacterAction::CoolingFromAttack()
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	AIController& ai_controller = ecs->GetComponentRef(AIController, entity);

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
	EntityCoordinator* ecs = GameData::Get().ecs;

	if(Collider* collider = ecs->GetComponent(Collider, entity))
	{
		SetFlag(collider->flags, (u32)Collider::IgnoreAll);
	}

	if(Physics* physics = ecs->GetComponent(Physics, entity))
	{
		physics->speed.set(0.0f, 0.0f);
	}
}

void CharacterAction::ApplyMovementEase(int movement_factor, float dt)
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
	Physics& physics = ecs->GetComponentRef(Physics, entity);

	const int run_acceleration_factor = 1;
	const VectorI facing_direction = state.GetFacingDirection();
	physics.ApplyMovementEase(facing_direction.toFloat(), dt, run_acceleration_factor);
}

bool CharacterAction::CanCreateAttackCollider(Entity attack_collider)
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	const Animator& animator = ecs->GetComponentRef(Animator, entity);
	const Animation& animation = animator.GetActiveAnimation();
	if(animation.attackColliderFrameStart != -1)
	{
		return animator.frameIndex >= animation.attackColliderFrameStart && attack_collider == EntityInvalid;
	}

	return false;
}
bool CharacterAction::CanDestroyAttackCollider(Entity attack_collider)
{
	EntityCoordinator* ecs = GameData::Get().ecs;
	const Animator& animator = ecs->GetComponentRef(Animator, entity);
	const Animation& animation = animator.GetActiveAnimation();
	if(animation.attackColliderFrameEnd != -1)
	{
		return animator.frameIndex >= animation.attackColliderFrameEnd && attack_collider != EntityInvalid;
	}

	return false;
}