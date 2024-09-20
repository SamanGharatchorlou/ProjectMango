#pragma once

namespace Player
{
	ECS::Entity Get();
	ECS::Entity Spawn(const char* id, const char* config);
};