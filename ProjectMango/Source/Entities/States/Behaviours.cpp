#include "pch.h"
#include "Behaviours.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"

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

	static void PlayAttackVFX(Entity entity)
	{
		Animator& animator = GetComponentRef(Animator, entity);
		BehaviourState& state = GetComponentRef(BehaviourState, entity);
		Action::Enum action = animator.GetActiveAnimation()->action;	

		if(state.attackData.contains(action))
		{
			AttackStateData& asd = state.attackData[action];
			bool hit_frame = animator.frameIndex == asd.hitFrame;	
			if(hit_frame && !state.playedAttackVfx && !asd.attackVfx.empty())
			{
				// attack vfx e.g. fire coming of your sword
				const Transform& transform = GetComponentRef(Transform, entity);
				const VectorF pos =  transform.worldPosition + transform.size * asd.hitBoxPos;
				const VectorF size = transform.size * asd.hitBoxSize;
				const RectF attack_rect(pos, size);

				CreateVFX(asd.attackVfx.c_str(), attack_rect);

				state.playedAttackVfx = true;

				// hit vfx e.g. blood splatter
				const Faction& faction = GetComponentRef(Faction, entity);
				Entity target_entity = faction.GetTarget();
				if (target_entity != EntityInvalid)
				{
					RectF rect;
					rect.SetSize(123.0f, 97.5f); // todo: fix
					rect.SetCenter(GetPosition(target_entity));

					CreateVFX(asd.hitVfx.c_str(), rect);
				}

				state.playedHitVfx = true;
			}
		}
	}

	static void AttackUpdate(ECS::Entity entity, Action::Enum attack)
	{
		Animator& animator = GetComponentRef(Animator, entity);
		BehaviourState& state = GetComponentRef(BehaviourState, entity);
		
		PlayAttackVFX(entity);

		if(state.attackData.contains(attack))
		{
			AttackStateData& asd = state.attackData[attack];
			bool hit_frame = animator.frameIndex == asd.hitFrame;
			if( hit_frame && !state.didHit)
			{
				state.didHit = true;

				const Faction& faction = GetComponentRef(Faction, entity);
				Entity target_entity = faction.GetTarget();
				if(target_entity != EntityInvalid)
				{
					// todo: should be able to move this hit frame stuff into HealthSystem
					if(Health* health = GetComponent(Health, target_entity))
					{
						Damage& damage = AddComponent(Damage, target_entity);
						damage.value = asd.damage;
						//damage.hitFrame = asd.hitFrame;
						//damage.sourceEntity = entity;
					}

					// should i check for the correct attack here?
					// if(debuff component) do the debuff thing
					if(!asd.debuff.empty())
					{
						DebugPrint(Log, "Apply debuff %s", asd.debuff.c_str());

						ApplyStatusEffect(asd.debuff.c_str(), target_entity);
					}
				}
			}
		}

		if(animator.loopCount > 0)
		{
			const Animation* animation = animator.GetActiveAnimation();
			ASSERT(animation->action == attack, "Not the basic attack anim state in the basic attack update");

			// mark the attack as finished
			BehaviourState& state = GetComponentRef(BehaviourState, entity);
			state.attackFinishedTimeMS = GetTicksMS();
		}
	}


	// BasicAttack
	static void BasicAttackUpdate(ECS::Entity entity)
	{
		AttackUpdate(entity, Action::BasicAttack);
	}

	static void FollowUpAttackUpdate(ECS::Entity entity)
	{
		AttackUpdate(entity, Action::FollowUpAttack);
	}

	// Debuff
	static void DebuffAttackUpdate(ECS::Entity entity)
	{
		AttackUpdate(entity, Action::Debuff);
	}

	// Death
	static void DeathEnter(ECS::Entity entity)
	{
		RemoveComponent(Collider, entity);
		RemoveComponent(TurnState, entity);
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
			case ECS::Action::FollowUpAttack:
				map.updates[state] = Actor::FollowUpAttackUpdate;
				break;
			case ECS::Action::AttackWindUp:
			case ECS::Action::AttackRecovery:
				map.updates[state] = Actor::PlayAttackVFX;
				break;
			case ECS::Action::Debuff:
				map.updates[state] = Actor::DebuffAttackUpdate;
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