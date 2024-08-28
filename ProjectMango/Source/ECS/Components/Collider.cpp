#include "pch.h"
#include "Collider.h"

#if TRACK_COLLISIONS
#include "Debugging/DebugDraw.h"
#endif

namespace ECS 
{
	bool Collider::intersects(const RectF& _rect) const
	{
		return !(	rect.LeftPoint()  > _rect.RightPoint() || 
					rect.RightPoint() < _rect.LeftPoint()  || 
					rect.TopPoint()   > _rect.BotPoint()   || 
					rect.BotPoint()   < _rect.TopPoint());
	}

	bool Collider::intersects(const Collider& collider) const
	{
		return intersects(collider.rect);
	}

	bool Collider::contains(VectorF position) const 
	{
#if DEBUG_MODE
		bool a = position.x > rect.RightPoint();
		bool b = position.x < rect.LeftPoint();
		bool c = position.y > rect.BotPoint(); 
		bool d = position.y < rect.TopPoint();
		int z = 4;
#endif

		return !(	position.x > rect.RightPoint() || 
					position.x < rect.LeftPoint()  || 
					position.y > rect.BotPoint()   || 
					position.y < rect.TopPoint());
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

	#if TRACK_COLLISIONS
	void Collider::renderCollider()
	{
		if (didHit())
		{
			debugDrawRect(scaledRect(), RenderColour::Blue);
		}
		else if (gotHit())
		{
			debugDrawRect(scaledRect(), RenderColour::Red);
		}
		else
		{
			debugDrawRect(scaledRect(), RenderColour::LightGrey);
		}
	}
	#endif
}