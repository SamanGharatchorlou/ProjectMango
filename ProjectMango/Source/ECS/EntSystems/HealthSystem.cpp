#include "pch.h"
#include "HealthSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/GraphicComponents.h"
#include "Game/Camera/Camera.h"

namespace ECS
{
	void HealthSystem::Init() { }

	void HealthSystem::Update(float dt)
	{
 		for (Entity entity : entities)
		{
			Health& health = GetComponentRef(Health, entity);

			if (Damage* damage = GetComponent(Damage, entity))
			{
				bool hit_frame = true;
				if (Animator* animator = GetComponent(Animator, damage->sourceEntity))
					hit_frame = animator->frameIndex == damage->hitFrame || damage->hitFrame == 0;
				
				if (hit_frame)
				{
					bool did_hit = health.ApplyDamage(damage->value);
					if (did_hit)
					{
						VectorF position = GetPosition(entity);
						Camera::Get()->AddShake(damage->value * 0.5f, position);
					}

					RemoveComponent(Damage, entity);
				}

			}
		}
	}
}