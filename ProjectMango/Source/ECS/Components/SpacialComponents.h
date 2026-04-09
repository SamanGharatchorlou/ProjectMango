#pragma once

#include "ComponentHelpers.h"

struct STexture;

namespace ECS
{
	struct Collider;

	struct Transform
	{
		COMPONENT_TYPE(Transform)

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

		VectorF speed;
		float maxSpeed = 0.0f;	
		float acceleration = 0.0f;

		float drag = 0.0f;
		float mass = 1.0f;

		bool applyGravity = false;
		bool onFloor = false;

		void Init();
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

	struct Level
	{
		VectorF worldPos;
		VectorF size;

		u32 index = 0;

		BasicString id;

		std::vector<Layer> layers;
		std::vector<ECS::Entity> colliders;

		Grid<int> walkableTiles;

		// can i assign the entity a value? do i care? a string is probably fine

		std::unordered_map<BasicString, std::vector<EntityMetaData>> entities;

		VectorI GetTileIndex(VectorF position) const;
		RectF GetWalkableTileRect(VectorI index) const;
		RectF GetBounds() const;

		bool IsPointInBounds(VectorF world_position) const;
	};

	struct Biome
	{
		COMPONENT_TYPE(Biome)
		Biome();

		std::vector<Level> levels;

		VectorF aabb[2];

		static const Level* GetLevelFromIndex(u32 level_index);
		static const Level& GetLevel(ECS::Entity entity);
		static const Level& GetLevel(VectorF position);
		static const Level& GetVisibleLevel();

		static const Entity GetActive();
		static const Biome& GetActiveBiome();

		//static bool GetLevelSpawnPos(const char* spawn_id, VectorF& out_pos);
	};
}