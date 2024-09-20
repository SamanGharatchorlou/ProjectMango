#pragma once


struct RaycastResult
{
	ECS::Entity entity = ECS::EntityInvalid;
	float distance = -1;
	VectorF hitPosition;

	bool hasHit = false;
};

void Raycast(VectorF from, VectorF direction, float distance, RaycastResult& result, const std::vector<ECS::Entity>* ignored = nullptr, std::vector<u32>* collider_flags = nullptr);
