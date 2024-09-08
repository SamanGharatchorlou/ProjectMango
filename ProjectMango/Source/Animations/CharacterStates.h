#pragma once

enum class ActionState
{
	None,

	Idle,
	Walk,
	Run,

	Jump,
	Fall,

	Roll,

	BasicAttack,
	BasicAttackHold,
	LungeAttack,

	TakeHit,
	Death,

	Count
};


ActionState StringToAction(const char* action);
const char* ActionToString(ActionState action);

#define PushState(action) state.actions.Push(new action##State(entity))
#define PopState() state.actions.Pop()
#define ReplaceState(action) state.actions.Replace(new action##State(entity))