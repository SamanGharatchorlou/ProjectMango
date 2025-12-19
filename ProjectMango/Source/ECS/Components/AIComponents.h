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

		bool CanMoveForward(int ease_factor, float dt) const;
	};

	struct Pathing
	{
		COMPONENT_TYPE(Pathing)

		Pathing();

		//Entity target;

		// the incremental next position to move to: pos + speed
		// probably set by the AIController
		//VectorF currentLocation;
		VectorF targetLocation;

		// sets the bounds
		//u32 levelIndex = -1;

		bool hasValidPath = false;

		//void Init();
	};

	struct AIIntent
	{
		COMPONENT_TYPE(AIIntent)

		bool wantsToFaceTarget = false;
		bool wantsToMove = false;
		bool wantsToAttack = false;

	};
	
	typedef void (*BehaviourFunction)( ECS::Entity );

	// wrap this and the state into 1 comp?
	struct BehaviourMap
	{
		COMPONENT_TYPE(BehaviourMap)

		std::unordered_map<Action::Enum, BehaviourFunction> updates;
		std::unordered_map<Action::Enum, BehaviourFunction> exits;
	};

	// pass/get this data when running behaviours from the behaviour map
	struct BehaviourState
	{
		COMPONENT_TYPE(BehaviourState)

		float acceleration;

		u64 attackCooldownTimeMS;
		u64 attackFinishedTimeMS;

		void Init();
	};
}