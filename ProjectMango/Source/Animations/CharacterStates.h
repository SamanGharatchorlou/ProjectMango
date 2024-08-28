#pragma once

#include <unordered_map> // could remove this by moving it into .cpp
#include "Core/StringBuffers.h"

class Texture;

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

	Death,

	Count
};

static std::unordered_map<StringBuffer32, ActionState> s_actionMap;

static void initActionMap()
{
	s_actionMap.reserve((size_t)ActionState::Count);

	s_actionMap["None"] = ActionState::None;

	s_actionMap["Idle"] = ActionState::Idle;
	s_actionMap["Walk"] = ActionState::Walk;
	s_actionMap["Run"] = ActionState::Run;
	
	s_actionMap["Jump"] = ActionState::Jump;
	s_actionMap["JumpMovingUp"] = ActionState::JumpMovingUp;
	s_actionMap["JumpMovingDown"] = ActionState::JumpMovingDown;

	s_actionMap["Roll"] = ActionState::Roll;

	s_actionMap["BasicAttack"] = ActionState::BasicAttack;
	s_actionMap["BasicAttackHold"] = ActionState::BasicAttackHold;
	s_actionMap["ChopAttack"] = ActionState::ChopAttack;

	s_actionMap["Death"] = ActionState::Death;
}

//
//#define ActionStateCase(action) case ActionState::action: \
//								out_size = sizeof(action##State);  \
//								state = new action##State[count]; \
//								break;

static ActionState stringToAction(const char* action)
{
	if (s_actionMap.empty())
		initActionMap();

	return s_actionMap.at(action);
}

static StringBuffer32 actionToString(ActionState action)
{
	if (s_actionMap.empty())
		initActionMap();

	for (auto iter = s_actionMap.begin(); iter != s_actionMap.end(); iter++)
	{
		if (iter->second == action)
			return iter->first;
	}

	return StringBuffer32();
}