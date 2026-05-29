#include "pch.h"
#include "ComponentHelpers.h"

#include "Components.h"
#include "GameComponents.h"
#include "GraphicComponents.h"
#include "SpacialComponents.h"
#include "ECS/EntityCoordinator.h"

namespace ECS
{
	const char* EntityMetaData::GetID() const
	{
		return data.GetString("id");
	}

	Entity CreateEntity(const ECS::EntityMetaData& emd)
	{
		Entity entity = ecs->CreateNewEntity();
		if ( emd.data.Contains("id") )
		{ 
			EntityData& ed = AddComponent(EntityData, entity); 
			ed.id = emd.GetID();
		}
		return entity;
	}

	Entity CreateEntity(const char* id, bool config_postfix)
	{
		Entity entity = ecs->CreateNewEntity();
		if (id)
		{
			EntityData& ed = AddComponent(EntityData, entity);
			ed.id = id;
		}

		return entity;
	}

	Entity FindEntityFromIid(u64 iid)
	{
		const ComponentArray<EntityData>& eds = GetAllComponents(EntityData);
		for (auto iter = eds.entityToComponent.begin(); iter != eds.entityToComponent.end(); iter++)
		{
			const EntityData& ed = eds.GetComponentByIndex(iter->second);
			if (ed.iid == iid)
				return iter->first;
		}

		DebugPrint(Warning, "No entity found with iid %u", iid);
		return EntityInvalid;
	}

	const char* GetName(Entity entity)
	{
		if(EntityData* ed = GetComponent(EntityData, entity))
			return ed->id.c_str();

		return nullptr;
	}

	const Config* GetConfigFromEntity(Entity entity)
	{
		if (const EntityData* ed = GetComponent(EntityData, entity))
		{
			if(!ed->id.empty())
				return ConfigManager::Get()->GetConfig(ed->id.c_str());
		}

		return nullptr;
	}

	Entity GetParent(Entity child)
	{
		if(EntityData* ed = GetComponent(EntityData, child))
		{
			return ed->parent;
		}

		return EntityInvalid;
	}

	Entity GetFirstChild(Entity parent)
	{
		if(EntityData* ed = GetComponent(EntityData, parent))
		{
			if(ed->children.size() > 0)
				return ed->children.front();
		}

		return EntityInvalid;
	}

	void DestroyChildren(Entity parent)
	{
		if(EntityData* ed = GetComponent(EntityData, parent))
		{
			// create a copy so the list doesnt change as we're looping it
			std::vector<Entity> children = ed->children;
			for( Entity child : children )
			{
				ecs->entities.KillEntity(child);
			}

			ed->children.clear();
		}
	}

	void GetChildren(Entity parent, std::vector<Entity>& children)
	{
		if(EntityData* ed = GetComponent(EntityData, parent))
		{
			for (u32 i = 0; i < ed->children.size(); i++)
			{
				children.push_back(ed->children[i]);
			}
		}
	}

	VectorF GetPosition(Entity entity)
	{	
		if(Transform* transform = GetComponent(Transform, entity))
		{
			return transform->GetObjectCenter();
		}

		return VectorF::zero();
	}

	void SetWorldPosition(ECS::Entity entity, VectorF pos)
	{
		if(Transform* transform = GetComponent(Transform, entity))
		{
			transform->SetWorldPosition(pos);
		}
	}

	RectF GetRect(Entity entity)
	{		
		if(const Collider* collider = GetComponent(Collider, entity))
		{
			return collider->rect;
		}
		else if(Transform* transform = GetComponent(Transform, entity))
		{
			return transform->GetRect();
		}

		return RectF();
	}

	bool GetRotationParams(Entity entity, VectorF& out_aboutPoint, float& out_rotation)
	{
		if (const Transform* transform = GetComponent(Transform, entity))
		{
			if (const ECS::Sprite* sprite = GetComponent(Sprite, entity))
			{
				out_rotation = sprite->params.rotation;

				RectF rect = transform->GetRect();
				out_aboutPoint = rect.TopLeft() + transform->GetHorizontalFlipPoint();
				return true;
			}
		}

		return false;
	}

	SDL_RendererFlip GetFacingDirection(Entity entity)
	{	
		Transform& transform = GetComponentRef(Transform, entity);
		return transform.facingDirection;
	}

	VectorI GetFacingDirectionVector(Entity entity)
	{
		return FacingDirectionToVector(GetFacingDirection(entity));
	}

	VectorI FacingDirectionToVector(SDL_RendererFlip facing)
	{
		int direction = (facing == SDL_FLIP_HORIZONTAL) ? -1 : 1;
		return VectorI(direction, 0);
	}

	void SetFacingDirection(Entity entity, SDL_RendererFlip direction)
	{
		Transform& transform = GetComponentRef(Transform, entity);
		if(transform.facingDirection != direction)
		{
			FlipFacingDirection(entity);
		}
	}

	SDL_RendererFlip FlipFacingDirection(Entity entity)
	{
		Transform& transform = GetComponentRef(Transform, entity);

		if(transform.facingDirection == SDL_FLIP_NONE)
			transform.facingDirection = SDL_FLIP_HORIZONTAL;
		else
			transform.facingDirection = SDL_FLIP_NONE;

		if(Sprite* sprite = GetComponent(Sprite, entity))
		{
			if (sprite->params.flip == SDL_FLIP_HORIZONTAL)
				sprite->params.flip = SDL_FLIP_NONE;
			else
				sprite->params.flip = SDL_FLIP_HORIZONTAL;
		}

		return transform.facingDirection;
	}

	SDL_RendererFlip GetDesiredFacingDirection(Entity entity, Entity target_entity)
	{
		VectorF self = GetPosition(entity);
		VectorF target = GetPosition(target_entity);
		return (target.x > self.x) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
	}

	void Resize(RectF& rect_a, const RectF& rect_b)
	{
		if (!rect_a.isValid() || !rect_b.isValid())
			return;

		float width_ratio = rect_a.Width() / rect_b.Width();
		float height_ratio = rect_a.Height() / rect_b.Height();

		VectorF size = rect_a.Size() / Maths::Max(width_ratio, height_ratio);
		rect_a.SetSize(size);
	}

	//bool IsInLevel(const Level& level, const Transform& transform)
	//{
	//	const VectorF position = transform.worldPosition;
	//	const VectorF world_pos = level.worldPos;

	//	if (position.x > world_pos.x && position.y > world_pos.y)
	//	{
	//		const VectorF world_pos_end = world_pos + level.size;
	//		return position.x < world_pos_end.x && position.y < world_pos_end.y;
	//	}

	//	return false;
	//}

	//void GetEntitiesInLevel(const Level& level, const std::unordered_map<Entity, u32>& in_entities, std::vector<Entity>& out_entities)
	//{
	//	for (auto iter = in_entities.begin(); iter != in_entities.end(); iter++)
	//	{
	//		ECS::Entity entity = iter->first;
	//		if (const Transform* transform = GetComponent(Transform, entity))
	//		{
	//			if (IsInLevel(level, *transform))
	//			{
	//				out_entities.push_back(entity);
	//			}
	//		}
	//	}
	//}

	// Actions
	const char* ActionToString(Action::Enum action)
	{
		static const char* names[] = {
			#define X(name) #name,
			ACTION_LIST
			#undef X
		};
		return names[action];
	}

	Action::Enum StringToAction(const char* str)
	{
		static const char* names[] = {
			#define X(name) #name,
			ACTION_LIST
			#undef X
		};

		for(int i = 0; i < Action::Count; i++)
			if(strcmp(names[i], str) == 0)
				return (Action::Enum)i;

		return Action::None;
	}
}