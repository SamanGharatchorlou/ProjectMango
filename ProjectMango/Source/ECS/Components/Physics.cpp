#include "pch.h"
#include "Physics.h"

#include "Core/Helpers.h"
#include "System/Files/Config.h"
#include "ComponentHelpers.h"

namespace ECS
{
	Physics::Physics() : mass(1.0f), applyGravity(false), onFloor(false) { }

	void Physics::Init()
	{	
		if(const Config* config = GetConfigFromEntity(entity))
		{
			applyGravity = config->data.GetBool("gravity");	
			//acceleration = config->data.GetFloat("acceleration");
			maxSpeed = config->data.GetFloat("max_run_speed");
		}

		drag = 5.0f;
		speed = VectorF::zero();
	}
}