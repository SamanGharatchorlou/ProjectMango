#include "pch.h"
#include "Maths.h"

namespace Maths
{
	// easings from https://easings.net/
	float EaseOutCubic(float x)
	{
		x = clamp<float>(x, 0.0f, 1.0f);
		return 1.0f - pow(1.0f - x, 3.0f);
	}
}