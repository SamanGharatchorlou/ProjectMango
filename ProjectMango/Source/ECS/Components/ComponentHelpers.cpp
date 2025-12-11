#include "pch.h"
#include "ComponentHelpers.h"
#include "Components.h"
#include "ECS/EntityCoordinator.h"
#include "Collider.h"
#include "Biome.h"

namespace ECS
{
	Entity CreateEntity(const ECS::EntityMetaData& emd)
	{
		Entity entity = ecs->CreateNewEntity();
		if (!emd.id.empty()) 
		{ 
			EntityData& ed = AddComponent(EntityData, entity); 
			ed.id = emd.id; 
			ed.subType = emd.type; 
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

	const char* GetName(Entity entity)
	{
		if(EntityData* ed = GetComponent(EntityData, entity))
			return ed->id.c_str();

		return nullptr;
	}

	const Config* GetConfigFromEntity(Entity entity)
	{
		const Config* config = nullptr;
		if (const EntityData* ed = GetComponent(EntityData, entity))
		{

			config = ConfigManager::Get()->GetConfig(ed->id.c_str());
		}

		return config;
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

	VectorF GetPosition(Entity entity)
	{	
		if(Transform* transform = GetComponent(Transform, entity))
		{
			return transform->GetObjectCenter();
		}

		return VectorF::zero();
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
				out_rotation = sprite->rotation;

				RectF rect = transform->GetRect();
				out_aboutPoint = rect.TopLeft() + (rect.Size() * sprite->flipPoint);
				return true;
			}
		}

		return false;
	}

	SDL_RendererFlip GetFacingDirection(Entity entity)
	{
		Sprite& sprite = GetComponentRef(Sprite, entity);
		return sprite.flip;
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
		Sprite& sprite = GetComponentRef(Sprite, entity);

		if (sprite.canFlip)
			sprite.flip = direction;
	}

	void FlipFacingDirection(Entity entity)
	{
		Sprite& sprite = GetComponentRef(Sprite, entity);

		if (sprite.canFlip)
		{
			if (sprite.flip == SDL_FLIP_HORIZONTAL)
				sprite.flip = SDL_FLIP_NONE;
			else
				sprite.flip = SDL_FLIP_HORIZONTAL;
		}
	}

	SDL_RendererFlip GetDesiredFacingDirection(Entity entity, Entity target_entity)
	{
		VectorF self = GetPosition(entity);
		VectorF target = GetPosition(target_entity);
		return (target.x > self.x) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
	}
}