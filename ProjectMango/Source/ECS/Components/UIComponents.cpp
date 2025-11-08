#include "pch.h"
#include "UIComponents.h"

#include "ECS/EntityCoordinator.h"

namespace ECS
{
	UICursor::UICursor() : cursor(nullptr) { }

	UICursor::~UICursor() { }

	UICursor* UICursor::Get()
	{
		ComponentArray<UICursor>& cursors = GetAllComponents(UICursor);
		if (cursors.Count() > 0)
		{
			auto front_index = cursors.entityToComponent.begin();
			return &cursors.GetComponentByIndex(front_index->second);
		}

		return nullptr;
	}
}