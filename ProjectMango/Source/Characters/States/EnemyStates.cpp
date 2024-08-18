#pragma once
#include "pch.h"
#include "EnemyStates.h"

#include "Game/States/State.h"
#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/AIController.h"
#include "ECS/Components/Physics.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "ECS/EntSystems/AnimationSystem.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/ComponentCommon.h"
#include "Core/Helpers.h"

namespace Enemy
{
	CharacterAction* StatePool::createNewObjects(ActionState type, int count, int& out_size) const
	{
		CharacterAction* state = nullptr;

		switch (type)
		{
		ActionStateCase(Idle)
		ActionStateCase(Run)
		ActionStateCase(BasicAttack)
		ActionStateCase(Death)
		case ActionState::Count:
		case ActionState::None:
		default:
			DebugPrint(Warning, "Invalid effect type %d, could not create %d objects\n", type, count);
			break;
		}

		return state;
	}

	bool IsInAttackRange(ECS::Entity attacker, ECS::Entity target, VectorI& out_facing_direction)
	{
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;

		VectorF position = ECS::GetPosition(attacker);
		VectorF target_position = ECS::GetPosition(target);

		VectorF target_facing = target_position - position;

		const ECS::Collider& target_collider = ecs->GetComponentRef(Collider, target);

		for( u32 i = 0; i < ECS::Direction::Count; i++ )
		{
			const VectorI direction = ECS::s_directions[i];
			const VectorI facing_direction = direction *-1;

			const RectF area = Enemy::GetAttackRect(attacker, facing_direction);
			if(target_collider.intersects(area))
			{
				out_facing_direction = facing_direction;
				return true;
			}
			
		}

		return false;
	}

	static bool CanAttackTarget(ECS::Entity entity, VectorI& out_facing_direction )
	{
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		if( ECS::Pathing* pathing = ecs->GetComponent(Pathing, entity) )
		{
			if(ecs->IsAlive(pathing->target))
			{
				if(IsInAttackRange(entity, pathing->target, out_facing_direction))
				{
					return true;
				}
			}
		}
		
		return false;
	}

	// Idle
	// ---------------------------------------------------------
	void IdleState::Update(float dt)
	{
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		ECS::AIController& ai = ecs->GetComponentRef(AIController, entity);
		ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
		
		//VectorI facing_direction;
		//if(CanAttackTarget(entity, facing_direction))
		//{
		//	ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
		//	state.facingDirection = facing_direction;

		//	ECS::AIController& ai = ecs->GetComponentRef(AIController, entity);
		//	ai.PushState(ActionState::BasicAttack);
		//}

		//if(!state.movementDirection.isZero())
		//{
		//	ai.PushState(ActionState::Run);
		//}

		//if (ECS::Physics* physics = ecs->GetComponent(Physics, entity))
		//{
		//	physics->speed.set(0.0f, 0.0f);
		//	physics->ApplyDrag(state.movementDirection.toFloat(), 0.9f);
		//}
	}

	// Run
	// ---------------------------------------------------------
	void RunState::Update(float dt)
	{
		//ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		//ECS::AIController& ai = ecs->GetComponentRef(AIController, entity);
		//const ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

		//if (ECS::Physics* physics = ecs->GetComponent(Physics, entity))
		//{
		//	// apply walk speed
		//	physics->maxSpeed = VectorF(3.0f, 3.0f);
		//	physics->ApplyMovement(state.movementDirection.toFloat(), dt);
		//	physics->ApplyDrag(state.movementDirection.toFloat(), 0.9f);
		//}

		//if(state.movementDirection.isZero())
		//{
		//	ai.PopState();
		//}
	}

	// BasicAttack
	// ---------------------------------------------------------
	void BasicAttackState::Init()
	{	
		//// init values
		//attackCollider = ECS::EntityInvalid;

		//ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		//const ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
		//const RectF character_rect = ECS::GetObjectRect(entity);

		//if(ECS::Physics* physics = ecs->GetComponent(Physics, entity))
		//{
		//	physics->speed.set(0.0f, 0.0f);
		//}
		//
		//const VectorI direction = state.facingDirection;
		//const RectF collider_rect = GetAttackRect(entity, direction);
		//attackCollider = CreateAttackCollider(entity, collider_rect, 60, "Enemy Attack Collider");
	}

	void BasicAttackState::Update(float dt)
	{
		//ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		//ECS::Animation& animation = ecs->GetComponentRef(Animation, entity);
		//ECS::AIController& ai = ecs->GetComponentRef(AIController, entity);
		//
		//const Animation* anim = animation.animator.activeAnimation();
		//
		//if(HandleAttackAnimation(entity, attackCollider))
		//{
		//	ai.PopState();
		//	ai.PushState(ActionState::Idle, 1.0f);
		//}
	}

	void BasicAttackState::Exit()
	{
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		GameData::Get().ecs->entities.KillEntity(attackCollider);
	}

	// DeathState
	// ---------------------------------------------------------
	void DeathState::Init()
	{
		can_kill = false;

		ECS::EntityCoordinator* ecs = GameData::Get().ecs;

		if(ECS::Collider* collider = ecs->GetComponent(Collider, entity))
		{
			SetFlag(collider->mFlags, (u32)ECS::Collider::IgnoreAll);
		}

		if(ECS::Physics* physics = ecs->GetComponent(Physics, entity))
		{
			physics->speed.set(0.0f, 0.0f);
		}
	}

	void DeathState::Update(float dt)
	{	
		//ECS::EntityCoordinator* ecs = GameData::Get().ecs;

		//const ECS::Animation& animation = ecs->GetComponentRef(Animation, entity);
		//if(animation.animator.finished())
		//{
		//	can_kill = true;
		//}
	}

	RectF GetAttackRect(ECS::Entity entity, const VectorI facing_direction)
	{
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		const ECS::CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

		const RectF character_rect = ECS::GetObjectRect(entity);
		const VectorF size = GetAttackArea(entity, facing_direction);

		RectF rect;
		rect.SetSize(size);
		
		if( facing_direction.x == 1 )
			rect.SetLeftCenter(character_rect.BotCenter());
		else if( facing_direction.x == -1 )
			rect.SetRightCenter(character_rect.BotCenter());
		else if( facing_direction.y == 1 )
			rect.SetTopCenter(ECS::GetColliderRect(entity).BotCenter());
		else if( facing_direction.y == -1)
			rect.SetBotCenter(character_rect.Center());

		return rect;
	}
	
	VectorF GetAttackArea(ECS::Entity entity, const VectorI facing_direction)
	{
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		const RectF boundary_rect = ECS::GetRenderRect(entity);
		
		if( facing_direction.x == -1 || facing_direction.x == 1 )
		{
			const float range = boundary_rect.Width() * 0.5f;
			const float height = boundary_rect.Height() * 0.25f;
			return VectorF(range, height);
		}
		else if( facing_direction.y == -1 || facing_direction.y == 1 )
		{
			const float range = boundary_rect.Height() * 0.3f;
			const float width = boundary_rect.Width() * 0.6f;
			return VectorF(width, range);
		}

		ASSERT(false, "We need some direction here");
		return VectorF();
	}

	float GetAttackRange(ECS::Entity entity, const VectorI facing_direction)
	{
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;

		const RectF character_rect = ECS::GetColliderRect(entity);
		const RectF attack_rect = GetAttackRect(entity, facing_direction);

		if( facing_direction.x == 1 )
		{
			return attack_rect.RightCenter().x - character_rect.Center().x;
		}
		else if( facing_direction.x == -1 )
		{
			return character_rect.Center().x - attack_rect.LeftCenter().x;
		}
		else if( facing_direction.y == 1 )
		{
			return attack_rect.BotCenter().y - character_rect.Center().y;
		}
		else if( facing_direction.y == -1)
		{
			return character_rect.Center().y - attack_rect.TopCenter().y;
		}

		return 0;
	}
	
	VectorF GetAttackRangeArea(ECS::Entity entity, const VectorI facing_direction)
	{
		const float attack_range = GetAttackRange(entity, facing_direction);
		return VectorF(attack_range, attack_range);
	}
}


