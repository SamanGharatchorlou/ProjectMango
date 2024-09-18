#pragma once

typedef std::unordered_map<StringBuffer32, float> SettingValues;

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

		void Init(const SettingValues& values);

		void ApplyMovement(VectorF movement_direction, float dt);
		void ApplyDrag(float drag_coefficient);

		void ApplyMovementEase(VectorF movement_direction, float dt, int easing_factor);
	};
}