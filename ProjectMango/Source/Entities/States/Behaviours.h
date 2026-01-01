#pragma once

#include "ECS/Components/ComponentHelpers.h"

//void BuildBehaviourMap();

namespace ECS
{
	struct BehaviourMap;
}

void PopulateDefaultBehaviours(ECS::BehaviourMap& map);
void PopulateMonsterBehaviours(ECS::BehaviourMap& map);