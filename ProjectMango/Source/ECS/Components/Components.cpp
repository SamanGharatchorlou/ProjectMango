#include "pch.h"
#include "Components.h"

#include "ECS/EntityCoordinator.h"



namespace ECS
{
	void Health::ApplyDamage(const Damage& damage)
	{
		if(invulnerable)
			return;

		EntityCoordinator* ecs = GameData::Get().ecs;

		for( u32 i = 0; i < ignoredDamaged.size(); i++ )
		{
			// ignore damage from this source
			if(ignoredDamaged[i] == damage.entity)
				return;
		}

		currentHealth -= damage.value;
		currentHealth = std::clamp(currentHealth, 0.0f, maxHealth);

		// todo: clean this list when the entity is dead
		ignoredDamaged.push_back(damage.entity);
	}

	//void Pathing::UpdateTargetPosition()
	//{
	//	EntityCoordinator* ecs = GameData::Get().ecs;
	//	if(ecs->IsAlive(target))
	//	{
	//		const RectF target_rect = ECS::GetColliderRect(target);

	//		VectorF start_position = ECS::GetPosition(entity);

	//		int min_path_length = INT_MAX;

	//		// check which position is most desirable out of top, right, bottom, left depending on our position
	//		for( u32 i = 0; i < Direction::Count; i++ )
	//		{
	//			const VectorI direction = s_directions[i];
	//			const VectorI facing_direction = direction *-1;
	//			
	//			VectorF pos = target_rect.Center();
	//			const VectorF area = Enemy::GetAttackRangeArea(entity, facing_direction);
	//			pos += area * direction.toFloat();

	//			if(DebugMenu::GetSelectedEntity() == entity)
	//				DebugMenu::SetPathAttackPoint(pos, (Direction)i);

	//			int path_length = PathingSystem::FindPath(start_position, pos).size();
	//			if(path_length < min_path_length)
	//			{
	//				min_path_length = path_length;
	//				currentTarget = pos;
	//			}
	//		}
	//	}
	//}
}