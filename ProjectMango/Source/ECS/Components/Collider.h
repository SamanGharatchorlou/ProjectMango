#pragma once

#include "ECS/EntityCommon.h"

namespace ECS
{
	struct Collider
	{
		COMPONENT_TYPE(Collider)

		enum Flags
		{
			// type
			Static = 1 << 0,

			// id of the collider
			IsPlayer = 1 << 2,
			IsEnemy = 1 << 3,

			// filtering
			HitPlayerOnly = 1 << 4,
			HitEnemyOnly = 1 << 5,

			// use to enable/disable collisions
			IgnoreAll = 1 << 6,
			IgnoreCollisions = 1 << 7,
			IgnoreDamage = 1 << 8,

			// to check for collisions without any affect
			GhostCollider = 1 << 9
		};

		enum RuntimeFlags 
		{ 
			RestrictLeft	= 1 << 0, 
			RestrictRight	= 2 << 0, 
			RestrictUp		= 3 << 0, 
			RestrictDown	= 4 << 0
		};
		
		virtual bool intersects(const Collider& collider) const;
		virtual bool intersects(const RectF& rect) const;
		bool contains(VectorF position) const;
	
		static bool test1DOverlap(float minA, float maxA, float minB, float maxB);

		void SetBaseRect(const RectF& rect);
		void SetTopLeft(const VectorF& pos);
		void SetRelativeRect(VectorF position, VectorF size);

		void RollBackPosition();
		void RollForwardPosition();

	#if TRACK_COLLISIONS
		virtual void renderCollider();	
	#endif
		
		u32 flags = 0;
		int lastHitFrame = -1;

		VectorF allowedMovement;

		VectorF forward;
		VectorF back;

		RectF rect; 

		std::vector<ECS::Entity> collisions;


	private:
		// dont use this, use the rect
		// the base rect represents the size of the transform, since the collider might be smaller 
		// we use the relative pos and size to build the actual rect of this collider from the transform
		RectF baseRect;

		// relative to the transform rect
		VectorF relative_position = VectorF(0,0);
		VectorF relative_size = VectorF(1,1);

		void UpdateRectFromBase();
	};
}