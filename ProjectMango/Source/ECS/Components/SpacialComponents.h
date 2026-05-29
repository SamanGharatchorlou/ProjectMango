#pragma once

#include "ComponentHelpers.h"

struct STexture;

namespace ECS
{
	struct Collider;

	struct Transform
	{
		COMPONENT_TYPE(Transform)

		static constexpr const char* kRequirement = "size";

		Transform();

		// top left
		VectorF targetWorldPosition;
		VectorF worldPosition;
		VectorF localPosition;

		VectorF size;

		// facing to the right ( 0 == right, 1 == left)
		SDL_RendererFlip facingDirection = SDL_FLIP_NONE;

		bool ignoreOutOfBounds;
		
		void Init(const EntityMetaData* emd, Collider& collider);
		void Init(const EntityMetaData* emd);
		void Init(const EntityMetaData& emd);
		void Serialise(EntityMetaData& out_emd) const;

		void SetLocalPosition(VectorF pos);
		void SetWorldPosition(VectorF pos);
		void SetWorldRect(const VectorF& pos, const VectorF& size);

		void UpdateChildTransforms();

		void SetObjectCenter(VectorF pos);
		RectF GetObjectRect() const;

		bool FacingLeft() const { return facingDirection == SDL_FLIP_HORIZONTAL; }

		VectorF GetHorizontalFlipPoint() const;
		VectorF GetObjectCenter() const;
		RectF GetRect() const;

		VectorF GetRelativePosition(VectorF relative) const;

		static VectorF GetObjectCenter(ECS::Entity entity);
	};

	struct Collider
	{
		COMPONENT_TYPE(Collider)

		static constexpr const char* kRequirement = "object_size";

		Collider();

		enum Flags
		{
			// base type
			None = 0,

			Static,
			Kinematic,

			// collider type
			IsPlayer,
			IsEnemy,
			IsTerrain,
			IsWall,
			IsFloor,
			IsDamage,

			// use to enable/disable collisions
			IgnoreAll,
			IgnoreDamage,
			IgnorePlayer,
			IgnoreTerrain,

			// only collider with terrain
			TerrainOnly,
			PlayerOnly,

			// to check for collisions without any affect, i.e. not physical
			GhostCollider,

			// helps prevent collider getting stuck, maybe just for player?
			// or only for more complex moving colliders maybe
			CanBump,

			QuadCollider
		};

		bool Intersects(const RectF& rect) const;
		bool Contains(VectorF position) const;

		static bool Contains(const RectF& rect, VectorF point);
		static bool Intersects(const RectF& rect_a, const RectF& rect_b);
		static bool Intersects(const Quad2D<float>& quad, const RectF& rect);
	
		static bool Test1DOverlap(float minA, float maxA, float minB, float maxB);

		void SetBaseRect(const RectF& rect);
		void SetTopLeft(const VectorF& pos);
		void SetRelativeRect(VectorF position, VectorF size);

		VectorF GetRealPosition(VectorF relative_pos) const;
		RectF GetRelativeRect() const;

		// assumes the rect size has been set
		void Init();
		void Init(const EntityMetaData& emd);
		void UpdateFromTransform(const Transform& transform);

		void RollBackPosition();
		void RollForwardPosition();

		inline bool HasFlag(Flags flag) const { return flags & (1 << flag); }
		inline void SetFlag(Flags flag) { flags |= (1 << flag); }
		inline void RemoveFlag(Flags flag) { flags &= ~(1 << flag); }

		bool HasCollided() const { return collisions.size() > 0; }

		bool initialised;
		VectorF alignmentOffset;

		// does collide: top, left, bot, right
		enum Side { Top, Right, Bottom, Left, Sides };
		bool collisionSide[Sides];

		bool destroyOnContact;
		int reboundCount;

		u32 flags;
		int lastHitFrame;

		VectorF allowedMovement;
		VectorF desiredMovement; // same as allowed but not edited based on collisions, more for tracking

		VectorF forward;
		VectorF back;

		RectF rect;
		// can replace rect if you need rotation by setting flag and this, can only check against a rect (not another quad)
		QuadF quad;

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


	struct Physics
	{
		COMPONENT_TYPE(Physics)

		static constexpr const char* kRequirement = "max_speed";

		VectorF speed;
		float maxSpeed = 0.0f;	
		float acceleration = 0.0f;

		float drag = 0.0f;
		float mass = 1.0f;

		bool applyGravity = false;
		bool onFloor = false;

		void Init();
		void Init(const EntityMetaData& emd);
	};


	struct TileSet
	{
		STexture* texture = nullptr;

		VectorF mapSize;
		VectorF tileSize;
	};

	struct Layer
	{
		struct Tile
		{
			VectorF draw_pos;
			VectorF tileset_pos;
		};

		std::vector<Tile> tiles;
		VectorF tileSize;

		TileSet* tileSet = nullptr;
	};

	struct Biome
	{
		COMPONENT_TYPE(Biome)
		Biome() { }

		std::vector<Layer> layers;
		std::vector<ECS::Entity> colliders;
		Grid<int> walkableTiles;

		// [type, list of entities of said type]
		std::vector<EntityMetaData> entityMetaData;

		VectorF biomeToWindow;
		//VectorF worldPos;
		VectorF size;

		u32 biomeIndex;

		BasicString id;

		VectorI GetTileIndex(VectorF position) const;
		RectF GetWalkableTileRect(VectorI index) const;
		RectF GetBounds() const;

		bool IsPointInBounds(VectorF world_position) const;

		static const Biome& GetActive();
	};
}