#pragma once


namespace ECS
{
	struct Physics
	{
		COMPONENT_TYPE(Physics)
		Physics();

		VectorF speed;
		float maxSpeed;	
		float acceleration;

		float drag;
		float mass;

		bool applyGravity;
		bool onFloor;

		void Init();
	};
}