#pragma once


struct RaycastResult
{
	ECS::Entity entity = ECS::EntityInvalid;
	float distance = -1;
	VectorF hitPosition;
};

void Raycast(VectorF from, VectorF direction, float distance, RaycastResult& result, const std::vector<ECS::Entity>* ignored = nullptr);
