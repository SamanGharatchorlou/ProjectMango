#include "pch.h"
#include "SpacialComponents.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/GraphicComponents.h"
#include "ECS/EntSystems/TransformSystem.h"

#include "Game/Camera/Camera.h"
#include "Core/Helpers.h"
#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"

namespace ECS 
{
		// Transform
	// ------------------------------------------------------------------
	Transform::Transform() : 
		ignoreOutOfBounds(false)
	{ }

	void Transform::Init(const EntityMetaData* emd)
	{	
		if(emd)
		{	
			size = emd->data.GetVector(kRequirement);
			SetWorldPosition(emd->data.GetVector("position") - (size * emd->data.GetVector("pivot_point")));
		}

		if(const Config* config = GetConfigFromEntity(entity))
		{
			size = config->data.GetVector("size");
			size = AdjustToScreenSize(size);
			facingDirection = config->data.GetBool("flipped") ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
		}
	}


	void Transform::Init(const EntityMetaData& emd)
	{
		size = emd.data.GetVector(kRequirement);
		
		// todo: do i need this?
		//size = AdjustToScreenSize(size);

		SetWorldPosition(emd.data.GetVector("position") - (size * emd.data.GetVector("pivot_point")));
	}
	
	void Transform::Init(const EntityMetaData* emd, Collider& collider)
	{
		Init(emd);
		collider.Init();
	}

	void Transform::Serialise(EntityMetaData& out_emd) const
	{
		out_emd.data.AddVectorF("position", worldPosition);
		out_emd.data.AddVectorF("size", size);
	}

	void Transform::SetWorldRect(const VectorF& _pos, const VectorF& _size)
	{
		size = _size;
		SetWorldPosition(_pos);
	}

	void Transform::UpdateChildTransforms()
	{
		// update children positions
		if (EntityData* entity_data = GetComponent(EntityData, entity))
		{
			// handle horizontal flip 
			for (u32 i = 0; i < entity_data->children.size(); i++)
			{
				Entity child = entity_data->children[i];
				Transform& child_transform = GetComponentRef(Transform, child);
				VectorF child_world_pos = worldPosition + child_transform.localPosition;

				if(child_transform.facingDirection != facingDirection)
				{
					// if the parent is flipped also flip the child about the parents flip point
					VectorF flip_distance = child_transform.localPosition - GetHorizontalFlipPoint();
					flip_distance.y = 0;
					child_world_pos -= (flip_distance * 2.0f + VectorF(child_transform.size.x, 0.0f));

					FlipFacingDirection(child);
				}

				child_transform.SetWorldPosition(child_world_pos);
			}
		}
	}

	void Transform::SetLocalPosition(VectorF pos)
	{
		ASSERT(!size.isZero(), "Make sure to set the size BEFORE you set the local position");

		localPosition = pos;
		
		// must have a parent by this point
		EntityData& entity_data = GetComponentRef(EntityData, entity);
		const Transform& parent_transform = GetComponentRef(Transform, entity_data.parent);
		SetWorldPosition(parent_transform.worldPosition + localPosition);

		UpdateChildTransforms();
	}

	void Transform::SetWorldPosition(VectorF pos)
	{
		targetWorldPosition = pos;
		worldPosition = pos;
		
		// update collider positions
		if (Collider* collider = GetComponent(Collider, entity))
		{
			collider->UpdateFromTransform(*this);
		}

		EntityData& entity_data = GetComponentRef(EntityData, entity);
		if(entity_data.parent != EntityInvalid)
		{
			const Transform& parent_transform = GetComponentRef(Transform, entity_data.parent);
			localPosition = worldPosition - parent_transform.worldPosition;
		}
	}

	void Transform::SetObjectCenter(VectorF pos)
	{
		VectorF object_size = size;
		// assuming a mid center
		VectorF object_offset = object_size * VectorF(0.5f, 0.5f);

		if(Collider* collider = GetComponent(Collider, entity))
		{
			if(!collider->initialised)
				DebugPrint(Warning, "Collider has not been init'd, has no size");

			object_offset = collider->rect.Center() - worldPosition;
		}

		SetWorldPosition(pos - object_offset);
	}

	
	RectF Transform::GetObjectRect() const
	{
		if(const Collider* collider = GetComponent(Collider, entity))
		{
			return collider->rect;
		}
		else
		{
			return GetRect();
		}
	}

	
	VectorF Transform::GetHorizontalFlipPoint() const
	{
		if(const Collider* collider = GetComponent(Collider, entity))
		{
			RectF relative_rect = collider->GetRelativeRect();
			VectorF flip = relative_rect.TopLeft() + (relative_rect.Size() * 0.5f);
			return VectorF(flip.x, 0.5f) * size;
		}
		else
		{
			return VectorF(0.5f, 0.5f) * size;
		}
	}

	VectorF Transform::GetObjectCenter() const
	{
		if(const Collider* collider = GetComponent(Collider, entity))
		{
			return collider->rect.Center();
		}
		else
		{
			return worldPosition + size * 0.5f;
		}

		return VectorF::zero();
	}

	VectorF Transform::GetObjectCenter(ECS::Entity entity)
	{
		const Transform& transform = GetComponentRef(Transform, entity);
		return transform.GetObjectCenter();
	}
	
	RectF Transform::GetRect() const
	{
		return RectF(worldPosition, size);
	}

	VectorF Transform::GetRelativePosition(VectorF relative) const
	{
		return worldPosition + (relative * size);
	}


	// Collider
	// ------------------------------------------------------------------
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

	void Collider::Init(const EntityMetaData& emd)
	{
		const Transform& transform = GetComponentRef(Transform, entity);
		ASSERT(!transform.size.isZero(), "cannot init collider when transform has no size");

		SetBaseRect(RectF(transform.worldPosition, transform.size));
		UpdateFromTransform(transform);

		VectorF object_position = emd.data.GetVector("object_pos");
		VectorF object_size = emd.data.GetVector(kRequirement, VectorF(1, 1));
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

	
	// Physics
	// ------------------------------------------------------------------
	void Physics::Init()
	{	
		if(const Config* config = GetConfigFromEntity(entity))
		{
			applyGravity = config->data.GetBool("gravity");	
			maxSpeed = config->data.GetFloat("max_speed");
		}

		drag = 5.0f;
		speed = VectorF::zero();
	}

	void Physics::Init(const EntityMetaData& emd)
	{
		maxSpeed = emd.data.GetFloat(kRequirement);
		applyGravity = emd.data.GetBool("gravity");
		drag = 5.0f;
		speed = VectorF::zero();
	}
	
	// Biome
	// ------------------------------------------------------------------
	const Biome& Biome::GetActive()
	{
		ComponentArray<Biome>& biomes = GetAllComponents(Biome);
		for (auto iter = biomes.entityToComponent.begin(); iter != biomes.entityToComponent.end(); iter++)
		{
			return biomes.GetComponentByIndex(iter->second);
		}

		DebugPrint(Error, "No biome is active");
		return Biome();
	}

	VectorI Biome::GetTileIndex(VectorF position) const
	{
		VectorF tile_size = layers.front().tileSize;
		return (position / tile_size).toInt();
	}

	
	RectF Biome::GetWalkableTileRect(VectorI index) const 
	{
		VectorF tile_size = layers.front().tileSize;
		VectorF tile_pos = index.toFloat() * tile_size;

		return RectF(tile_pos, tile_size);
	}

	RectF Biome::GetBounds() const
	{
		return RectF(VectorF::zero(), size);
	}

	bool Biome::IsPointInBounds(VectorF world_position) const
	{
		return Contains(GetBounds(), world_position);
	}
}