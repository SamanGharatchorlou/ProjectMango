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

		const RectF& GetRect() const;
		void SetRect(const RectF& rect);

		//void SetSt

		void RollBackPosition();
		void RollForwardPosition();

	#if TRACK_COLLISIONS
		virtual void renderCollider();	
	#endif
		
		u32 mFlags = 0;
		
		VectorF allowedMovement;

		VectorF mForward;
		VectorF mBack;
		//VectorF posOffset;

		std::vector<ECS::Entity> collisions;

	private:
		// always need to use the transform to set this position and offset by posOffset
		RectF mRect;
	};
}