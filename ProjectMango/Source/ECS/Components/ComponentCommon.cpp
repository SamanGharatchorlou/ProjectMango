#include "pch.h"
#include "ComponentCommon.h"

#include "ECS/EntityCoordinator.h"
#include "Components.h"

namespace ECS
{
	RectF GetRenderRect(Entity entity)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		RectF rect;
		//if(const Sprite* sprite = ecs->GetComponent(Sprite, entity))
		//{
		//	rect.SetSize(sprite->renderSize);
		//	if(const Transform* transform = ecs->GetComponent(Transform, entity))
		//	{
		//		rect.SetCenter(transform->rect.Center());
		//	}
		//}

		return rect;
	}

	
	RectF GetObjectRect(Entity entity)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		
		RectF rect;
		
		//if(Animation* animation = ecs->GetComponent(Animator, entity))
		//{
		//	const Animator& animator = animation->animator;
		//	if(const SpriteSheet* ss = animator.getActiveSpritesheet())
		//	{
		//		rect.SetSize(ss->objectSize);

		//		if(const Transform* transform = ecs->GetComponent(Transform, entity))
		//		{
		//			rect.SetTopLeft(transform->rect.TopLeft() + ss->objectPos);
		//		}
		//	}
		//}

		return rect;
	}

	RectF GetColliderRect(Entity entity)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		
		RectF rect;
		
		//if(Animation* animation = ecs->GetComponent(Animation, entity))
		//{
		//	const Animator& animator = animation->animator;
		//	if(const SpriteSheet* ss = animator.getActiveSpritesheet())
		//	{
		//		rect.SetSize(ss->colliderSize);

		//		if(const Transform* transform = ecs->GetComponent(Transform, entity))
		//		{
		//			rect.SetTopLeft(transform->rect.TopLeft() + ss->colliderPos);
		//		}
		//	}
		//}

		return rect;
	}

	// but this might be unreliable tbh
	// collider is probably the most accurate thing to represent its position
	VectorF GetPosition(Entity entity)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		if (const Transform* transform = ecs->GetComponent(Transform, entity))
		{
			return transform->position;// GetPos();
		}

		return VectorF::zero();
	}
}