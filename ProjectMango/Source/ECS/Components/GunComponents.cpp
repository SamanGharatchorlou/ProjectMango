#include "pch.h"
#include "GunComponents.h"

#include "Entities/Weapons/GunEntityBuilder.h"

#include "Game/FrameRateController.h"

namespace ECS
{
	Firearm::Firearm() : lastFireTime(0), reloadTime(1.0f), timeBetweenShots(0.2f)
	{

	}

	Entity Firearm::Fire()
	{
		const FrameRateController& frc = FrameRateController::Get();
		float game_time = frc.GameSeconds();

		bool can_shoot = game_time > (lastFireTime + timeBetweenShots);
		if(can_shoot)
		{
			lastFireTime = game_time;
			return CreateBasicBullet(*this);
		}

		return EntityInvalid;
	}
}

