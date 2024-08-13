#pragma once

namespace ECS
{
	void RegisterAllComponents();
	void RegisterAllSystems();

	void RemoveAllComponents(Entity entity);
}