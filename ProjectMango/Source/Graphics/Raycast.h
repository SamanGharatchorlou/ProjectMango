#pragma once

struct RaycastResult
{
	ECS::Entity entity = ECS::EntityInvalid;
	float distance = FLT_MAX;
	VectorF hitPosition;

	bool hasHit = false;
};

// each increment is 4 pixles, so make it a little bigger
static float constexpr c_minRayIncrement = 4.1f;

void Raycast(VectorF from, VectorF direction, float distance, RaycastResult& result, const std::vector<ECS::Entity>* ignored = nullptr, std::vector<u32>* collider_flags = nullptr);

bool RaycastToFloor(ECS::Entity entity, RaycastResult& result);
bool RaycastToFloor(const RectF& rect, float& out_distance);
bool RaycastToFloor(const VectorF& start, RaycastResult& result);
bool RaycastToWall(ECS::Entity entity, VectorF direction, RaycastResult& out_result);
