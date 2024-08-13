#pragma once

namespace ECS
{
	struct Physics
	{
		COMPONENT_TYPE(Physics)

		VectorF speed;
		VectorF maxSpeed;	
		VectorF acceleration;

		bool applyGravity;

		void ApplyMovement(VectorF movement_direction, float dt);
		void ApplyDrag(VectorF movement_direction, float drag_coefficient);
	};
}