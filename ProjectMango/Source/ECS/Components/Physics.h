#pragma once


namespace ECS
{
	struct Physics
	{
		COMPONENT_TYPE(Physics)
		Physics();

		VectorF speed;
		VectorF maxSpeed;	
		VectorF acceleration;

		float drag;
		float mass;

		bool applyGravity;
		bool onFloor;

		void Init();
	};
}