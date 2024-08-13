#pragma once

namespace Maths
{
	constexpr float pi = 3.14159f;

	// including min, not max (min, max]
	static int randomNumberBetween(int min, int max)
	{
		return (rand() % (max - min)) + min;
	}

	template<typename T>
	static T clamp(const T value, const T min, const T max)
	{
		return std::min(max, std::max(value, min));
	}

	template<typename T>
	static T toRadians(const T degrees)
	{
		return degrees * static_cast<T>(pi / 180.0f);
	}

	template<typename T>
	static T toDegrees(const T radians)
	{
		return radians * static_cast<T>(180.0f / pi);
	}
}