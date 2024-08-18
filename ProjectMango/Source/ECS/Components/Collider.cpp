#include "pch.h"
#include "Collider.h"

#if TRACK_COLLISIONS
#include "Debugging/DebugDraw.h"
#endif

namespace ECS 
{
	bool Collider::intersects(const RectF& rect) const
	{
		return !(	rect.LeftPoint()  > mRect.RightPoint() || 
					rect.RightPoint() < mRect.LeftPoint()  || 
					rect.TopPoint()   > mRect.BotPoint()   || 
					rect.BotPoint()   < mRect.TopPoint());
	}

	bool Collider::intersects(const Collider& collider) const
	{
		return intersects(collider.mRect);
	}

	bool Collider::contains(VectorF position) const 
	{
#if DEBUG_MODE
		bool a = position.x > mRect.RightPoint();
		bool b = position.x < mRect.LeftPoint();
		bool c = position.y > mRect.BotPoint(); 
		bool d = position.y < mRect.TopPoint();
		int z = 4;
#endif

		return !(	position.x > mRect.RightPoint() || 
					position.x < mRect.LeftPoint()  || 
					position.y > mRect.BotPoint()   || 
					position.y < mRect.TopPoint());
	}

	bool Collider::test1DOverlap(float minA, float maxA, float minB, float maxB)
	{
		return maxA > minB && minA < maxB;
	}

	const RectF& Collider::GetRect() const
	{
		return mRect;
	}

	void Collider::SetRect(const RectF& rect)
	{
		mRect = rect;
	}

	//void Collider::SetPosition(const RectF& rect, VectorF& forward)
	//{
	//	SetCenter(rect.Center());
	//	mForward = forward;
	//	mBack = rect.Center();
	//}
	void Collider::RollBackPosition()
	{
		mRect.SetCenter(mBack);
	}
	void Collider::RollForwardPosition()
	{
		mRect.SetCenter(mForward);
	}
		
	//void Collider::SetCenter(const VectorF& pos)
	//{
	//	mRect.SetCenter(pos + posOffset);
	//}

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