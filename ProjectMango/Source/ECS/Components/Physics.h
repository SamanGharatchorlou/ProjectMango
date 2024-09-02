#pragma once

namespace ECS
{
	struct Physics
	{
		COMPONENT_TYPE(Physics)

		VectorF speed;
		VectorF maxSpeed;	
		VectorF acceleration;

		float mass = 1.0f;

		bool applyGravity;
		bool onFloor;

		void ApplyMovement(VectorF movement_direction, float dt);
		void ApplyDrag(float drag_coefficient);

		void ApplyMovementEase(VectorF movement_direction, float dt, int easing_factor);
	};
}