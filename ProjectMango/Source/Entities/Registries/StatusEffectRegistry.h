#pragma once

namespace ECS { struct StatusEffect; }

namespace StatusEffectRegistry
{
	void PopulateRegistry();

	void OnApply(ECS::StatusEffect& effect, ECS::Entity target);
	//void CreateStatusEffect(const char* effect, ECS::StatusEffect& out_effect);
}