#include "pch.h"
#include "CameraShake.h"

// doesnt work, ends at an offset, need to come back to x = 0
static float easeInOutBack(float x)
{
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;

	return x < 0.5f
	  ? (pow(2.0f * x, 2.0f) * ((c2 + 1.0f) * 2.0f * x - c2)) / 2.0f
	  : (pow(2.0f * x - 2.0f, 2.0f) * ((c2 + 1) * (x * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
}

static float sine(float x)
{
	return sin((double)x);
}

void CameraShake::Update(float dt)
{
	if(x < 1.0f)
	{
		float sdirection = direction.x >= 0 ? 1 : -1;
		float progress = sine(x * M_PI * 2.0f ) * sdirection;
		float damping = 1 - (x * 0.5f);
		VectorF amplitude = maxTrauma * magnitude;
		trauma = amplitude * progress * damping;

		x += dt * speed;
	}
	else
	{
		trauma = VectorF::zero();
		magnitude = 0;
	}

	return;
}