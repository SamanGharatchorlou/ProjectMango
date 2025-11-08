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

		float lastFireTime;
		float reloadTime;
		float timeBetweenShots;

		Entity Fire();
	};
}