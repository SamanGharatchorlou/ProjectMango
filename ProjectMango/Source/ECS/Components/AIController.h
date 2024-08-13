#pragma once

#include "Core/stack.h"
#include "Characters/States/EnemyStates.h" // can try to forward decalare state

namespace ECS
{
	static float constexpr c_noTime = 0.0f;

	struct AIController
	{
		COMPONENT_TYPE(AIController)

		~AIController();

		Enemy::StatePool statePool;
		ActionStack<CharacterAction> actions;

		float lockStateTime = c_noTime;


		bool PushState(ActionState state);
		bool PushState(ActionState state, float lock_in_state_time);
		void PopState();

	private:
		bool CanPushTimedState();
		bool PushNewState(ActionState state);
	};
}