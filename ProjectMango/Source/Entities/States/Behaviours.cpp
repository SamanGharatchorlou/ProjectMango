#include "pch.h"
#include "Behaviours.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Game/Camera/Camera.h"

//temp
#include "Entities/EntityBuilder.h"

using namespace ECS;

// default behaviours
namespace Actor
{
	// Idle
	static void IdleUpdate(ECS::Entity entity)
	{

	}

	// Run
	static void RunUpdate(ECS::Entity entity)
	{
		const BehaviourState& state = GetComponentRef(BehaviourState, entity);

		Physics& physics = GetComponentRef(Physics, entity);

		const SDL_RendererFlip flip_direction = GetFacingDirection(entity);
		const VectorI facing_direction = FacingDirectionToVector(flip_direction);
		physics.acceleration = facing_direction.toFloat().x * state.accelleration;
		ASSERT(state.accelleration > 0, "Accelleration == 0, but its trying to run");
	}
	static void RunExit(ECS::Entity entity)
	{
		Physics& physics = GetComponentRef(Physics, entity);
		physics.acceleration = 0;
	}

	
	static void PlayHitAnimationVFX(Entity entity)
	{
		Animator& animator = GetComponentRef(Animator, entity);
		BehaviourState& state = GetComponentRef(BehaviourState, entity);
		Action::Enum action = animator.GetActiveAnimation().action;

		if(state.attackData.contains(action))
		{
			AttackStateData& asd = state.attackData[action];
			bool hit_frame = animator.frameIndex == asd.hitFrame;
			if( hit_frame && !asd.playedHitVfx && !asd.hitVfx.empty())
			{
				const Faction& faction = GetComponentRef(Faction, entity);
				Entity target_entity = faction.GetTarget();
				if(target_entity != EntityInvalid)
				{
					if(!asd.hitVfx.empty())
					{
						RectF rect;
						rect.SetSize(123.0f,97.5f);
						rect.SetCenter(GetPosition(target_entity));

						CreateVFX(asd.hitVfx.c_str(), rect);
					}

					VectorF position = GetPosition(target_entity);
					Camera::Get()->AddShake(0.5f, position);
				}
				
				asd.playedHitVfx = true;
			}
		}
	}

	static void PlayAttackAnimationVFX(Entity entity)
	{
		Animator& animator = GetComponentRef(Animator, entity);
		BehaviourState& state = GetComponentRef(BehaviourState, entity);
		Action::Enum action = animator.GetActiveAnimation().action;	

		if(state.attackData.contains(action))
		{
			AttackStateData& asd = state.attackData[action];
			bool hit_frame = animator.frameIndex == asd.hitFrame;	
			if(hit_frame && !asd.playedAttackVfx && !asd.attackVfx.empty())
			{
				const Transform& transform = GetComponentRef(Transform, entity);
				const VectorF pos =  transform.worldPosition + transform.size * asd.hitBoxPos;
				const VectorF size = transform.size * asd.hitBoxSize;
				const RectF attack_rect(pos, size);

				CreateVFX(asd.attackVfx.c_str(), attack_rect);

				asd.playedAttackVfx = true;
			}
		}
	}

	static void PlayAttackVFX(ECS::Entity entity)
	{		
		PlayAttackAnimationVFX(entity);
		PlayHitAnimationVFX(entity);
	}

	// BasicAttack
	static void BasicAttackUpdate(ECS::Entity entity)
	{
		Animator& animator = GetComponentRef(Animator, entity);
		BehaviourState& state = GetComponentRef(BehaviourState, entity);
		
		PlayAttackVFX(entity);

		if(state.attackData.contains(Action::BasicAttack))
		{

			AttackStateData& asd = state.attackData[Action::BasicAttack];
			bool hit_frame = animator.frameIndex == asd.hitFrame;
			if( hit_frame && !asd.didHit)
			{
				asd.didHit = true;

				const Faction& faction = GetComponentRef(Faction, entity);
				Entity target_entity = faction.GetTarget();
				if(target_entity != EntityInvalid)
				{
					if(const Damage* damage = GetComponent(Damage, entity))
					{
						if(Health* health = GetComponent(Health, target_entity))
						{
							health->ApplyDamage(damage->value);
						}
					}
				}
			}
		}

		if(animator.loopCount > 0)
		{
			const Animation& animation = animator.GetActiveAnimation();
			ASSERT(animation.action == Action::BasicAttack, "Not the basic attack anim state in the basic attack update");

			// mark the attack as finished
			BehaviourState& state = GetComponentRef(BehaviourState, entity);
			state.attackFinishedTimeMS = GetTicksMS();
		}
	}

	static void BasicAttackExit(ECS::Entity entity)
	{
		BehaviourState& state = GetComponentRef(BehaviourState, entity);
		if(state.attackData.contains(Action::BasicAttack))
		{
			AttackStateData& asd = state.attackData[Action::BasicAttack];
			asd.didHit = false;
		}
	}

	// Death
	static void DeathEnter(ECS::Entity entity)
	{
		RemoveComponent(Collider, entity);
	}
}


namespace Monster
{
	static void BasicAttackExit(ECS::Entity entity)
	{
		DeathScentence& ds = AddComponent(DeathScentence, entity);
		ds.deathLoops = 1;

		ds.deathTimer = 1.0f;
		ds.fadeOutTime = 1.0f;
	}
}

void PopulateDefaultBehaviours(ECS::BehaviourMap& map)
{
	for( u32 i = 0; i < Action::Count; i++ )
	{
		Action::Enum state = (Action::Enum)i;
		switch( state )
		{
			case ECS::Action::Idle:
				map.updates[state] = Actor::IdleUpdate;
				break;
			case ECS::Action::Run:
				map.updates[state] = Actor::RunUpdate;
				map.exits[state] = Actor::RunExit;
				break;
			case ECS::Action::BasicAttack:
				map.updates[state] = Actor::BasicAttackUpdate;
				break;
			case ECS::Action::AttackWindUp:
			case ECS::Action::AttackRecovery:
				map.updates[state] = Actor::PlayAttackVFX;
				break;
			case ECS::Action::Death:
				map.enters[state] = Actor::DeathEnter;
				break;
			default:
				break;
		}
	}
}

void PopulateMonsterBehaviours(ECS::BehaviourMap& map)
{
	// populate with defaults first
	PopulateDefaultBehaviours(map);

	// then override any defined with the monster versions
	for( u32 i = 0; i < Action::Count; i++ )
	{
		Action::Enum state = (Action::Enum)i;
		switch( state )
		{
			case ECS::Action::BasicAttack:
				map.exits[state] = Monster::BasicAttackExit;
				break;
			default:
				break;
		}
	}
}