#include "pch.h"
#include "Collider.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"

#if TRACK_COLLISIONS
#include "Debugging/DebugDraw.h"
#endif

namespace ECS 
{
	bool Collider::Contains(const RectF& rect, VectorF point)
	{
		return !(	point.x > rect.RightPoint() || 
					point.x < rect.LeftPoint()  || 
					point.y > rect.BotPoint()   || 
					point.y < rect.TopPoint());
	}

	bool Collider::Intersects(const RectF& rect_a, const RectF& rect_b)
	{
		return !(	rect_a.LeftPoint()  > rect_b.RightPoint() || 
					rect_a.RightPoint() < rect_b.LeftPoint()  || 
					rect_a.TopPoint()   > rect_b.BotPoint()   || 
					rect_a.BotPoint()   < rect_b.TopPoint());
	}

	bool Collider::intersects(const RectF& _rect) const
	{
		return Intersects(rect, _rect);
	}

	bool Collider::intersects(const Collider& collider) const
	{
		return Intersects(rect, collider.rect);
	}

	bool Collider::contains(VectorF position) const 
	{
		return Contains(rect, position);
	}

	bool Collider::test1DOverlap(float minA, float maxA, float minB, float maxB)
	{
		return maxA > minB && minA < maxB;
	}

	void Collider::UpdateRectFromBase()
	{
		VectorF rel_top_left = baseRect.TopLeft() + (baseRect.Size() * relative_position);
		VectorF rel_size = baseRect.Size() * relative_size;
		rect = RectF(rel_top_left, rel_size);
	}
	
	void Collider::SetRelativeRect(VectorF position, VectorF size)
	{
		relative_position = position;
		relative_size = size;

		UpdateRectFromBase();
	}

	void Collider::SetBaseRect(const RectF& _rect)
	{
		baseRect = _rect;
		forward = baseRect.TopLeft();
		back = forward;

		UpdateRectFromBase();
	}

	void Collider::SetTopLeft(const VectorF& pos)
	{
		baseRect.SetTopLeft(pos);		
		forward = baseRect.TopLeft();
		back = forward;

		UpdateRectFromBase();
	}

	// using these requires updating the GetRect
	void Collider::RollBackPosition()
	{
		baseRect.SetTopLeft(back);
		rect.SetTopLeft(baseRect.TopLeft() + (baseRect.Size() * relative_position));
	}
	void Collider::RollForwardPosition()
	{
		baseRect.SetTopLeft(forward);
		rect.SetTopLeft(baseRect.TopLeft() + (baseRect.Size() * relative_position));
	}

	
	void Collider::InitFromTransform(const Transform& transform)
	{
		SetBaseRect(RectF(transform.worldPosition, transform.size));
		UpdateFromTransform();
	}

	void Collider::UpdateFromTransform()
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		const Transform& transform = ecs->GetComponentRef(Transform, entity);

		back = transform.worldPosition;
		forward = transform.targetWorldPosition;
		RollForwardPosition();
	}
}