#pragma once

namespace ECS
{
	// contains bullets, shoot from this
	struct Magazine
	{
		BasicString bulletId;

		int capacity = 10;
		int remaining = 10;
	};

	struct Firearm
	{
		COMPONENT_TYPE(Firearm)
		
		Magazine magazine;

		u64 lastShotTimeMS;
		u64 shotTime;
		
		u64 reloadTimeMS;
		u64 lastReloadTimeMS;

		Entity Fire();
		void Reload();
	};
}