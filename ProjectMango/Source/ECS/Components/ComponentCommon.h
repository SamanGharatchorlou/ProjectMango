#pragma once

namespace ECS
{
	RectF GetRenderRect(Entity entity);
	RectF GetObjectRect(Entity entity);
	RectF GetColliderRect(Entity entity);

	VectorF GetPosition(Entity entity);
}