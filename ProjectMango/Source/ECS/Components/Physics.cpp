#include "pch.h"
#include "Physics.h"

#include "Core/Helpers.h"

using namespace ECS;

void Physics::Init(const SettingValues& values)
{	
	applyGravity = values.GetBool("gravity");	
	acceleration = values.GetVectorF("acceleration_x", "acceleration_y");
	maxSpeed = values.GetVectorF("max_run_speed", "max_fall_speed");

	speed = VectorF::zero();
}

void Physics::ApplyDrag(float drag_coefficient)
{
	const float dragFractor = 1.0f - drag_coefficient;
	const float minSpeedMultiple = 0.01f;

	speed.x = speed.x * dragFractor;

	// stop once we reach 1% of our max speed (should this be a fixed number?)
	const float min_speed = maxSpeed.x * minSpeedMultiple;
	if (speed.x < min_speed && speed.x > -min_speed)
		speed.x = 0;
}

void Physics::ApplyMovement(VectorF movement_direction, float dt)
{
	// only clamp our acceleration, i.e. if something pushes us past our max speed thats fine
	if(movement_direction.x > 0)
	{
		if(speed.x < maxSpeed.x)
		{
			float speed_increase = movement_direction.x * acceleration.x * dt;

			// double acceleration when moving in the other direction
			if(speed.x < 0.0f)
				speed_increase *= 2.0f;

			float available_increase = maxSpeed.x - speed.x;

			speed_increase = Maths::clamp(speed_increase, 0.0f, available_increase);
			speed.x += speed_increase;
 		}
	}
	else if(movement_direction.x < 0)
	{
		if(speed.x > -maxSpeed.x)
		{
			float speed_increase = movement_direction.x * acceleration.x * dt;

			// double acceleration when moving in the other direction
			if(speed.x > 0.0f)
				speed_increase *= 2.0f;

			float available_increase = -maxSpeed.x - speed.x;

			speed_increase = Maths::clamp(speed_increase, available_increase, 0.0f);
			speed.x += speed_increase;
 		}
	}
}


VectorF Physics::GetMovementEase(VectorF movement_direction, float dt, int easing_factor) const
{
	VectorF movement;
	if(movement_direction.x != 0)
	{
		float max_speed_x = movement_direction.x >= 0 ? maxSpeed.x : -maxSpeed.x;

		float tx = speed.x / max_speed_x;
		tx += dt; // * speed;
		tx = Maths::clamp(tx, 0.0f, 1.0f);

		movement.x = EaseOut(tx, easing_factor) * max_speed_x - speed.x;
	}

	if(movement_direction.y != 0)
	{
		float max_speed_y = movement_direction.y >= 0 ? maxSpeed.y : -maxSpeed.y;

		float ty = speed.y / max_speed_y;
		ty += dt; // * speed;
		ty = Maths::clamp(ty, 0.0f, 1.0f);

		movement.y = EaseOut(ty, easing_factor) * max_speed_y - speed.y;
	}

	return movement;
}

void Physics::ApplyMovementEase(VectorF movement_direction, float dt, int easing_factor)
{
	speed = speed + GetMovementEase(movement_direction, dt, easing_factor);
}