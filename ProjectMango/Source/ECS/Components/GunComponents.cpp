#include "pch.h"
#include "GunComponents.h"

#include "Entities/Weapons/GunEntityBuilder.h"

#include "Audio/AudioManager.h"
#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"

namespace ECS
{
	Firearm::Firearm() : lastShotTimeMS(0), shotTime(200), 
		lastReloadTimeMS(0), reloadTimeMS(1000)
	{

	}

	Entity Firearm::Fire()
	{
		bool shot_cooldown = GetTicksMS() > (lastShotTimeMS + shotTime);
		bool has_bullets = magazine.remaining > 0;

		if( shot_cooldown )
		{
			lastShotTimeMS = GetTicksMS();

			// play empty mag audio
			if(has_bullets)
			{
				magazine.remaining--;
				return CreateBasicBullet(*this);
			}
			if(Audio* audio = GetComponent(Audio, entity))
			{
				lastShotTimeMS += 100;
				audio->Play("empty");
			}
		}

		return EntityInvalid;
	}

	
	void Firearm::Reload()
	{
		bool can_reload = GetTicksMS() > (lastReloadTimeMS + reloadTimeMS);

		if(can_reload)
		{
			magazine.remaining = magazine.capacity;
			lastReloadTimeMS = GetTicksMS();
			lastShotTimeMS = 0;

			if(Audio* audio = GetComponent(Audio, entity))
			{
				audio->Play("reload");
			}
		}
	}
}

