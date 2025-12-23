#include "pch.h"
#include "Collider.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"

namespace ECS 
{
	Collider::Collider() : 
		initialised(false),
		flags(0),
		lastHitFrame(-1),
		destroyOnContact(false),
		reboundCount(0)
	{
		memset(collisionSide, 0, sizeof(bool) * Side::Sides);
	}

	bool Collider::Contains(const RectF& rect, VectorF point)
	{
		return !(	point.x > rect.RightPoint() || 
					point.x < rect.LeftPoint()  || 
					point.y > rect.BotPoint()   || 
					point.y < rect.TopPoint());
	}

	bool Collider::Intersects(const RectF& rect_a, const RectF& rect_b)
	{
		// strict
		bool a = rect_a.LeftPoint()  < rect_b.RightPoint();
		bool b = rect_a.RightPoint() > rect_b.LeftPoint();
		bool c = rect_a.TopPoint()   < rect_b.BotPoint();
		bool d = rect_a.BotPoint()   > rect_b.TopPoint();
		return a && b && c && d;

		//return !(	rect_a.LeftPoint()  > rect_b.RightPoint() || 
		//			rect_a.RightPoint() < rect_b.LeftPoint()  || 
		//			rect_a.TopPoint()   > rect_b.BotPoint()   || 
		//			rect_a.BotPoint()   < rect_b.TopPoint());
	}

	bool Collider::Intersects(const RectF& _rect) const
	{
		if (HasFlag(Flags::QuadCollider))
			return Intersects(quad, _rect);

		return Intersects(rect, _rect);
	}

	bool Collider::Contains(VectorF position) const 
	{
		return Contains(rect, position);
	}

	bool Collider::Test1DOverlap(float minA, float maxA, float minB, float maxB)
	{
		return maxA > minB && minA < maxB;
	}

	void Collider::UpdateRectFromBase()
	{
		VectorF rel_top_left = baseRect.TopLeft() + (baseRect.Size() * relative_position);
		VectorF rel_size = baseRect.Size() * relative_size;
		rect = RectF(rel_top_left, rel_size);

		if (HasFlag(QuadCollider))
		{
			float rotation = 0;
			VectorF about_point;
			ECS::GetRotationParams(entity, about_point, rotation);

			quad = Quad2D<float>(rect);
			quad.rotate(rotation, about_point);
		}
	}
	
	void Collider::SetRelativeRect(VectorF position, VectorF size)
	{
		relative_position = position;
		relative_size = size;

		UpdateRectFromBase();
	}


	VectorF Collider::GetRealPosition(VectorF relative_pos) const
	{
		return baseRect.TopLeft() + (baseRect.Size() * relative_pos);
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
		UpdateRectFromBase();
	}
	void Collider::RollForwardPosition()
	{
		baseRect.SetTopLeft(forward);
		UpdateRectFromBase();
	}
	
	void Collider::Init()
	{
		const Transform& transform = GetComponentRef(Transform, entity);
		ASSERT(!transform.size.isZero(), "cannot init collider when transform has no size");
		SetBaseRect(RectF(transform.worldPosition, transform.size));
		UpdateFromTransform(transform);

		VectorF object_position = VectorF::zero();
		VectorF object_size = VectorF(1, 1);
		if(const Config* config = GetConfigFromEntity(entity))
		{
			object_position = config->data.GetVector("object_pos", VectorF(0,0) );
			object_size = config->data.GetVector("object_size", VectorF(1, 1) );
		}
		SetRelativeRect(object_position, object_size);

		initialised = true;
	}

	void Collider::UpdateFromTransform(const Transform& transform)
	{
		back = transform.worldPosition;
		forward = transform.targetWorldPosition;

		RollForwardPosition();
	}

	bool Collider::Intersects(const QuadF& quad, const RectF& rect)
	{
		QuadF thatQuad(rect);

		// Assuming 'thatRect' is orientated on the x/y axis, test for overlaps
		bool xOverlap = Test1DOverlap(quad.xMin(), quad.xMax(), rect.LeftPoint(), rect.RightPoint());
		bool yOverlap = Test1DOverlap(quad.yMin(), quad.yMax(), rect.TopPoint(), rect.BotPoint());

		if (!xOverlap || !yOverlap)
			return false;

		// [quad][min,max]
		float projections[2][2];

		// No need to use the normal_hat vectors
		VectorF normals[2]{ quad.normal0(), quad.normal1() };
		const QuadF* quads[2]{ &quad, &thatQuad };

		// Project onto quad normals
		for (int n = 0; n < 2; n++)
		{
			for (int q = 0; q < 2; q++)
			{
				float projection = (*quads[q])[0].dot(normals[n]);
				float minProjection = projection;
				float maxProjection = projection;
				for (int i = 1; i < quads[q]->sides(); i++)
				{
					projection = (*quads[q])[i].dot(normals[n]);

					if (projection < minProjection)
						minProjection = projection;

					if (projection > maxProjection)
						maxProjection = projection;
				}

				projections[q][0] = minProjection;
				projections[q][1] = maxProjection;
			}

			bool nOverlap = Test1DOverlap(projections[0][0], projections[0][1], projections[1][0], projections[1][1]);

			if (!nOverlap)
				return false;
		}

		return true;
	}

	
	RectF Collider::GetRelativeRect() const
	{
		return RectF(relative_position, relative_size);
	}
}