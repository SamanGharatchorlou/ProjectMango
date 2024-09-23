#pragma once

enum class ActionState
{
	None,

	Active,
	Inactive,

	Open,
	Close,

	Idle,
	Walk,
	Run,
	
	Fall,
	Jump,
	Hover,

	Roll,

	AttackWindUp,
	BasicAttack,
	BasicAttackHold,
	LungeAttack,
	FloorSlam,

	TakeHit,
	Death,

	Count
};


ActionState StringToAction(const char* action);
const char* ActionToString(ActionState action);

#define PushState(action) state.actions.Push(new action##State(entity))
#define PopState() state.actions.Pop()
#define ReplaceState(action) state.actions.Replace(new action##State(entity))