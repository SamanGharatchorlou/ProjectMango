#include "pch.h"
#include "Physics.h"

using namespace ECS;

void Physics::ApplyDrag(float drag_coefficient)
{
	const float dragFractor = 1.0f - drag_coefficient;
	const float minSpeedMultiple = 0.1f;

	speed.x = speed.x * dragFractor;

	const float min_speed = maxSpeed.x * minSpeedMultiple;
	if (speed.x < min_speed && speed.x > -min_speed)
		speed.x = 0;
}


void Physics::ApplyMovement(VectorF movement_direction, float dt)
{
	// peeeeoooowwwwwwmmmm
	speed += movement_direction * acceleration * dt;
	speed.x = Maths::clamp(speed.x, maxSpeed.x * -1.0f, maxSpeed.x);
	speed.y = Maths::clamp(speed.y, maxSpeed.y * -1.0f, maxSpeed.y);

	//// handle diagonal movement
	//const float total_speed = speed.length();
	//const float max_speed = maxSpeed.x;
	//const float speed_ratio = total_speed / max_speed;
	//if (speed_ratio > 1.0f)
	//{
	//	speed /= speed_ratio;
	//}

}