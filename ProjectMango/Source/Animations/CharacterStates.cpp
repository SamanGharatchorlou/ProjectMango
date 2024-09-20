#include "pch.h"
#include "CharacterStates.h"
#include <unordered_map> // could remove this by moving it into .cpp

static std::unordered_map<StringBuffer32, ActionState> s_actionMap;

static void initActionMap()
{
	s_actionMap.reserve((size_t)ActionState::Count);

	s_actionMap["None"] = ActionState::None;

	s_actionMap["Active"] = ActionState::Active;

	s_actionMap["Idle"] = ActionState::Idle;
	s_actionMap["Walk"] = ActionState::Walk;
	s_actionMap["Run"] = ActionState::Run;
	
	s_actionMap["Fall"] = ActionState::Fall;
	s_actionMap["Jump"] = ActionState::Jump;
	s_actionMap["Hover"] = ActionState::Hover;

	s_actionMap["Roll"] = ActionState::Roll;

	s_actionMap["BasicAttack"] = ActionState::BasicAttack;
	s_actionMap["BasicAttackHold"] = ActionState::BasicAttackHold;
	s_actionMap["LungeAttack"] = ActionState::LungeAttack;
	s_actionMap["FloorSlam"] = ActionState::FloorSlam;
	
	s_actionMap["TakeHit"] = ActionState::TakeHit;
	s_actionMap["Death"] = ActionState::Death;
}

ActionState StringToAction(const char* action)
{
	if (s_actionMap.empty())
		initActionMap();

	return s_actionMap.at(action);
}

const char* ActionToString(ActionState action)
{
	if (s_actionMap.empty())
		initActionMap();

	for (auto iter = s_actionMap.begin(); iter != s_actionMap.end(); iter++)
	{
		if (iter->second == action)
			return iter->first.c_str();
	}

	return nullptr;
}