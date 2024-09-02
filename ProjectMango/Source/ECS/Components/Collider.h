#pragma once

#include "ECS/EntityCommon.h"

namespace ECS
{
	struct Collider
	{
		COMPONENT_TYPE(Collider)

			// todod stop the 1 << x thing and do that in the function call, make one for
			// colliders, this way i dont need to keep updating it like a mug
		enum Flags
		{
			// base type
			None = 0,
			Static,

			// collider type
			IsPlayer,
			IsEnemy,
			IsTerrain,
			IsDamage,

			// use to enable/disable collisions
			IgnoreAll,
			IgnoreDamage,

			// only collider with terrain
			TerrainOnly,

			// to check for collisions without any affect, i.e. not physical
			GhostCollider,

			// helps prevent collider getting stuck, maybe just for player?
			// or only for more complex moving colliders maybe
			CanBump
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

		inline bool HasFlag(Flags flag) const { return flags & (1 << flag); }
		inline void SetFlag(Flags flag) { flags |= (1 << flag); }
		inline void RemoveFlag(Flags flag) { flags &= ~(1 << flag); }

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