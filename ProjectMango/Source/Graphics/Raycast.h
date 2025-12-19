#pragma once

struct RaycastResult
{
	ECS::Entity entity = ECS::EntityInvalid;
	float distance = -1;
	VectorF hitPosition;

	bool hasHit = false;
};

void Raycast(VectorF from, VectorF direction, float distance, RaycastResult& result, const std::vector<ECS::Entity>* ignored = nullptr, std::vector<u32>* collider_flags = nullptr);

bool RaycastToFloor(ECS::Entity entity, float& out_distance);
bool RaycastToFloor(const RectF& rect, float& out_distance);
bool RaycastToFloor(const VectorF& start, RaycastResult& result);
bool RaycastToWall(ECS::Entity entity, VectorF direction, float& out_distance);
