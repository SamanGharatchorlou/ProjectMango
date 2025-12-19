#include "pch.h"
#include "Behaviours.h"
#include "ECS/Components/Components.h"

#include "ECS/Components/AIComponents.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/Animator.h"

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
		physics.acceleration = facing_direction.toFloat() * state.acceleration;
		ASSERT(state.acceleration > 0, "Accelleration == 0, but its trying to run");
	}
	static void RunExit(ECS::Entity entity)
	{
		Physics& physics = GetComponentRef(Physics, entity);
		physics.acceleration = VectorF(0,0);
	}

	// BasicAttack
	static void BasicAttackUpdate(ECS::Entity entity)
	{
		Animator& animator = GetComponentRef(Animator, entity);
		if(animator.loopCount > 0)
		{
			const Animation& animation = animator.GetActiveAnimation();
			ASSERT(animation.action == Action::BasicAttack, "Not the basic attack anim state in the basic attack update");

			// mark the attack as finished
			BehaviourState& state = GetComponentRef(BehaviourState, entity);
			state.attackFinishedTimeMS = GetTicksMS();
		}
	}
}


namespace Monster
{
	static void BasicAttackExit(ECS::Entity entity)
	{
		DeathScentence& ds = AddComponent(DeathScentence, entity);
		ds.deathLoops = 1;
	}
}

void PopulateDefaultBehaviours(ECS::BehaviourMap& map, std::vector<Action::Enum> actions)
{
	for( u32 i = 0; i < actions.size(); i++ )
	{
		Action::Enum state = actions[i];
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
			default:
				break;
		}
	}
}

void PopulateMonsterBehaviours(ECS::BehaviourMap& map, std::vector<Action::Enum> actions)
{
	// populate with defaults first
	PopulateDefaultBehaviours(map, actions);

	// then override any defined with the monster versions
	for( u32 i = 0; i < actions.size(); i++ )
	{
		Action::Enum state = actions[i];
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