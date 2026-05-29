#include "pch.h"
#include "Maths.h"

#include <random>
#include <sstream>
#include <iomanip>

namespace Maths
{
	// easings from https://easings.net/
	float EaseOutCubic(float x)
	{
		x = clamp<float>(x, 0.0f, 1.0f);
		return 1.0f - pow(1.0f - x, 3.0f);
	}

	u32 GenerateIID()
	{
		static std::mt19937 rng(std::random_device{}());
		static std::uniform_int_distribution<uint32_t> dist(0, 15);
		static std::uniform_int_distribution<uint32_t> dist8(8, 11);

		std::ostringstream ss;
		ss << std::hex;
		for (int i = 0; i < 8; i++) ss << dist(rng);
		ss << "-";
		for (int i = 0; i < 4; i++) ss << dist(rng);
		ss << "-4";  // version 4
		for (int i = 0; i < 3; i++) ss << dist(rng);
		ss << "-";
		ss << dist8(rng);  // variant bits
		for (int i = 0; i < 3; i++) ss << dist(rng);
		ss << "-";
		for (int i = 0; i < 12; i++) ss << dist(rng);

		std::size_t hash = std::hash<BasicString>{ }(ss.str().c_str());
		return static_cast<u32>(hash ^ (hash >> 32));
	}
}