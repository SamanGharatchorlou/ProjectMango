#include "pch.h"
#include "CallbackSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"

void SetupCallbackBindings(std::unordered_map<BasicString, std::function<void(ECS::Entity)>>& button_bindings);

namespace ECS
{
	std::unordered_map<BasicString, std::function<void(Entity)>> s_callbackBindings;

	// setup all text bindings
	void CallbackSystem::Init()
	{
		SetupCallbackBindings(s_callbackBindings);
	}

	void CallbackSystem::Update(float dt)
	{
		for (Entity entity : entities)
		{
			Callback& callback = GetComponentRef(Callback, entity);

			auto iter = s_callbackBindings.find(callback.callback.c_str());
			if(iter != s_callbackBindings.end())
				iter->second(entity);
		}
	}
} 