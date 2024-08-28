#pragma once

enum class ActionState
{
	None,

	Idle,
	Walk,
	Run,

	Jump,

	JumpMovingUp,
	JumpMovingDown,

	Roll,

	BasicAttack,
	BasicAttackHold,

	ChopAttack,

	TakeHit,
	Death,

	Count
};


ActionState StringToAction(const char* action);
const char* ActionToString(ActionState action);

#define PushNewState(action) state.actions.Push(new action##State(entity))