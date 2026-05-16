#pragma once

#include "Components.h"

// todo: rename file
namespace ECS
{
	struct AIController
	{
		COMPONENT_TYPE(AIController)

		// cant move or attack
		bool isDisabled = false;
	};

	struct Pathing
	{
		COMPONENT_TYPE(Pathing)

		// the incremental next position to move to: pos + speed
		// probably set by the AIController
		//VectorF currentLocation;
		VectorF targetLocation;

		bool hasValidPath = false;
	};

	struct AIIntent
	{
		COMPONENT_TYPE(AIIntent)

		bool wantsToBeInactive = false;

		bool wantsToFaceTarget = false;
		bool wantsToMove = false;
		bool wantsToAttack = false;
		bool wantsToDebuff = false;
	};


	struct EnemyPhase
	{
		enum Type
		{
			Approach, Attack, Debuff, Recover
		};

		Type type;
		int turnDuration = 1; // how many turns we stay in this phase
	};

	struct Strategy
	{
		BasicString name;
		std::vector<EnemyPhase> phases;
	};

	struct AIStrategy
	{
		COMPONENT_TYPE(AIStrategy)

		std::vector<Strategy> strategies;

		int currentStrategy = 0;
		int currentPhase = 0;
		int turnsLeft = 0;

		int currentPhaseEnteredFrame = 0;

		// phase
		void NextPhase();
		int GetNextPhase() const;
		EnemyPhase& GetCurrentPhase();
		const EnemyPhase& GetCurrentPhase() const;

		// strategy
		void NextStrategy();
	};
	
	typedef void (*BehaviourFunction)( ECS::Entity );

	// wrap this and the state into 1 comp?
	struct BehaviourMap
	{
		COMPONENT_TYPE(BehaviourMap)
			
		std::unordered_map<Action::Enum, BehaviourFunction> enters;
		std::unordered_map<Action::Enum, BehaviourFunction> updates;
		std::unordered_map<Action::Enum, BehaviourFunction> exits;

		bool attemptEnterFunction = true;
	};

	struct AttackStateData
	{
		// vfx of the attack i.e. the blade swing
		BasicString attackVfx;

		// vfx of the hit, i.e. blood splat
		BasicString hitVfx;

		// size of the attack collider, relative to the transform
		VectorF hitBoxPos = VectorF(0, 0);
		VectorF hitBoxSize = VectorF(1.0f, 1.0f);
		
		// how much of the total damage this does, i.e. attack + follow up attack can do 0.5 each
		float damage = 0.0f;

		BasicString debuff;

		// the frame at which the actual hit occours, take damage, fire vfx etc
		int hitFrame = 0;
	};

	// pass/get this data when running behaviours from the behaviour map
	struct BehaviourState
	{
		COMPONENT_TYPE(BehaviourState)

		float accelleration = 0.0f;

		// time the last attack finished and the cooldown begins
		u64 attackCooldownTimeMS = 0;
		u64 attackFinishedTimeMS = 0;
		
		// reset when we enter a new state
		bool didHit = false;
		bool playedAttackVfx = false;
		bool playedHitVfx = false;

		std::unordered_map<Action::Enum, AttackStateData> attackData;

		void Init();
		void Reset();

		int GetHitFrame(Action::Enum action);
	};
}