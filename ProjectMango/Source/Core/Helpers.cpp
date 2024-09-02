#include "pch.h"
#include "Helpers.h"



float EaseOut(float value, int easing_factor)
{
	return 1.0f - Power<float>(1.0f - value, easing_factor);
}