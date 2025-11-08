#pragma once

namespace PickUps
{
	typedef bool(*OnPickupFn)(ECS::Entity pickup_entity, ECS::Entity picker_upper);

	void SetCallbacks();
	OnPickupFn GetCallback(const char* pickup_item);

	//bool ApplyReboundRune(ECS::Entity rune_entity, ECS::Entity hit_entity);
	//bool ApplyEchoRune(ECS::Entity rune_entity, ECS::Entity hit_entity);
}
