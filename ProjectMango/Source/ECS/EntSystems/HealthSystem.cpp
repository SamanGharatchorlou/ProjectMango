#include "pch.h"
#include "HealthSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"

namespace ECS
{
	void HealthSystem::Init() { }

	void HealthSystem::Update(float dt)
	{
 		for (Entity entity : entities)
		{
			//const Health& health = GetComponentRef(Health, entity);

			//if(LayeredSprite* layered_sprite = GetComponent(LayeredSprite, entity))
			//{
			//	LayeredSprite::Layer& layer = layered_sprite->spriteLayers[1];
			//	const Transform& transform = GetComponentRef(Transform, entity);

			//	float remaining_health = 1 - (health.currentHealth / health.maxHealth);

			//	VectorF t_size = transform.size;

			//	VectorF pos = transform.worldPosition + VectorF(0.0f, t_size.y * (1-remaining_health));
			//	VectorF size = VectorF(t_size.x, t_size.y * remaining_health);
			//	layer.rect = RectF(pos, size);
			//}
		}
	}
}