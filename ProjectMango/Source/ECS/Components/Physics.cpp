#include "pch.h"
#include "Physics.h"

using namespace ECS;

void Physics::ApplyDrag(VectorF movement_direction, float drag_coefficient)
{
	const float dragFractor = 1.0f - drag_coefficient;
	const float minSpeedMultiple = 0.1f;

	// No movement, apply drag then halt at min speed
	if (!movement_direction.x)
	{
		speed.x = speed.x * dragFractor;

		const float min_speed = maxSpeed.x * minSpeedMultiple;
		if (speed.x < min_speed && speed.x > -min_speed)
			speed.x = 0;
	}
	if (!movement_direction.y)
	{
		speed.y = speed.y * dragFractor;

		const float min_speed = maxSpeed.y * minSpeedMultiple;
		if (speed.y < min_speed && speed.y > -min_speed)
			speed.y = 0;
	}

	// Changing direction
	if (speed.x > 0.0f && movement_direction.x < 0.0f ||
		speed.x < 0.0f && movement_direction.x > 0.0f)
	{
		speed.x = speed.x * dragFractor;
	}

	if (speed.y > 0.0f && movement_direction.y < 0.0f ||
		speed.y < 0.0f && movement_direction.y > 0.0f)
	{
		speed.y = speed.y * dragFractor;
	}
}


void Physics::ApplyMovement(VectorF movement_direction, float dt)
{
	// peeeeoooowwwwwwmmmm
	speed += movement_direction * acceleration * dt;
	speed = speed.clamp(maxSpeed * -1.0f, maxSpeed);

	// handle diagonal movement
	const float total_speed = speed.length();
	const float max_speed = maxSpeed.x;
	const float speed_ratio = total_speed / max_speed;
	if (speed_ratio > 1.0f)
	{
		speed /= speed_ratio;
	}

}