#pragma once

#include "ECS/Components/ComponentHelpers.h"

//void BuildBehaviourMap();

namespace ECS
{
	struct BehaviourMap;
}

void PopulateDefaultBehaviours(ECS::BehaviourMap& map, std::vector<ECS::Action::Enum> actions);
void PopulateMonsterBehaviours(ECS::BehaviourMap& map, std::vector<ECS::Action::Enum> actions);