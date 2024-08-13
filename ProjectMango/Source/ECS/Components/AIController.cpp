#pragma once
#include "pch.h"
#include "AIController.h"
#include "Game/FrameRateController.h"

namespace ECS
{
	
	AIController::~AIController()
	{
		int a = 4;
	}

	
	bool AIController::CanPushTimedState()
	{
		if(lockStateTime != c_noTime)
		{
			if(lockStateTime >= FrameRateController::Get().GameSeconds())
				return false;
		}

		return true;
	}

	bool AIController::PushNewState(ActionState action)
	{
		if(CharacterAction* state = statePool.getObject(action))
		{
			state->entity = entity;
			state->action = action;

			actions.Push(state);
			return true;
		}

		return false;
	}

	bool AIController::PushState(ActionState action)
	{
		if(!CanPushTimedState())
			return false;

		lockStateTime = c_noTime;

		// prevent multiple of the same states
		if(actions.HasAction() && actions.Top().action == action)
			return false;

		DebugPrint(PriorityLevel::Debug, "Pushing enemy action state: %s | %d states left", actionToString(action).c_str(), statePool.size(action));

		return PushNewState(action);
	}

	// push a state and lock it into the state for a certain time
	bool AIController::PushState(ActionState action, float lock_in_state_time)
	{
		if(CanPushTimedState())
		{
			lockStateTime = FrameRateController::Get().GameSeconds() + lock_in_state_time;
			return PushState(action);
		}

		return false;
	}

	void AIController::PopState()
	{
		CharacterAction* state = &actions.Top();
		actions.Pop();
		statePool.returnObject(state, state->action);

		DebugPrint(PriorityLevel::Debug, "Returning enemy action state: %s | %d states left", actionToString(state->action).c_str(), statePool.size(state->action));
	}
}