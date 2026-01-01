#include "pch.h"
#include "AIComponents.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/SpacialComponents.h"

namespace ECS
{	
	void BehaviourState::Init()
	{
		if(const Config* config = GetConfigFromEntity(entity))
		{
			accelleration = config->data.GetFloat("acceleration");
			attackCooldownTimeMS = (u64)config->data.GetInt("attack_cooldown_time_ms");
			//actionCooldownTimeMS = (u64)config->data.GetInt("action_cooldown_time_ms");
		}
	}
}