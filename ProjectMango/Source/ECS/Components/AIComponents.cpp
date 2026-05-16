#include "pch.h"
#include "AIComponents.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/SpacialComponents.h"
#include "Game/FrameRateController.h"

namespace ECS
{	
	void BehaviourState::Reset()
	{
		didHit = false;
		playedAttackVfx = false;
		playedHitVfx = false;
	}

	void BehaviourState::Init()
	{
		Reset();
		if(const Config* config = GetConfigFromEntity(entity))
		{
			accelleration = config->data.GetFloat("acceleration");
			attackCooldownTimeMS = (u64)config->data.GetInt("attack_cooldown_time_ms");
			//actionCooldownTimeMS = (u64)config->data.GetInt("action_cooldown_time_ms");
		}
	}


	int BehaviourState::GetHitFrame(Action::Enum action)
	{
		if (attackData.contains(action))
		{
			AttackStateData& asd = attackData[action];
			return attackData[action].hitFrame;
		}

		return 0;
	}

	int AIStrategy::GetNextPhase() const
	{
		const Strategy& pattern = strategies[currentStrategy];
		return (currentPhase + 1) % (int)pattern.phases.size(); 
	}

	void AIStrategy::NextPhase() 
	{
		currentPhase = GetNextPhase(); 
		turnsLeft = GetCurrentPhase().turnDuration;

		const FrameRateController& frc = FrameRateController::Get();
		currentPhaseEnteredFrame = FrameRateController::Get().frameCount;
	}
	
	EnemyPhase& AIStrategy::GetCurrentPhase()
	{
		return strategies[currentStrategy].phases[currentPhase];
	}

	const EnemyPhase& AIStrategy::GetCurrentPhase() const
	{
		return strategies[currentStrategy].phases[currentPhase];
	}

	void AIStrategy::NextStrategy()
	{
		currentStrategy = (currentStrategy + 1) % (int)strategies.size();
		currentPhase = 0;
		turnsLeft = GetCurrentPhase().turnDuration;
	}
}