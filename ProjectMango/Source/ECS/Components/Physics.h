#pragma once

struct Config;

namespace ECS
{
	struct Physics
	{
		COMPONENT_TYPE(Physics)

		VectorF speed;
		VectorF maxSpeed;	
		VectorF acceleration;

		float mass;

		bool applyGravity;
		bool onFloor;

		void Init(const Config* config);

		void ApplyMovement(VectorF movement_direction, float dt);
		void ApplyHorizontalDrag(float drag_coefficient);

		void ApplyMovementEase(VectorF movement_direction, float dt, int easing_factor);
		VectorF GetMovementEase(VectorF movement_direction, float dt, int easing_factor) const ;
	};
}