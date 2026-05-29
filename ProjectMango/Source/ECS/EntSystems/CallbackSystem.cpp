#include "pch.h"
#include "CallbackSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"

void SetupCallbackInits(std::unordered_map<BasicString, std::function<void(ECS::Entity)>>& callback_bindings);
void SetupCallbackUpdates(std::unordered_map<BasicString, std::function<void(ECS::Entity)>>& callback_bindings);

namespace ECS
{
	std::unordered_map<BasicString, std::function<void(Entity)>> s_callbackUpdate;
	std::unordered_map<BasicString, std::function<void(Entity)>> s_callbackInit;

	// setup all text bindings
	void CallbackSystem::Init()
	{
		SetupCallbackInits(s_callbackInit);
		SetupCallbackUpdates(s_callbackUpdate);
	}

	void CallbackSystem::Update(float dt)
	{
		for (Entity entity : entities)
		{
			Callback& callback = GetComponentRef(Callback, entity);

			if (callback.firstRun)
			{
				// see what i did there?
				auto initer = s_callbackInit.find(callback.callback.c_str());
				if (initer != s_callbackInit.end())
					initer->second(entity);

				callback.firstRun = false;
			}

			auto iter = s_callbackUpdate.find(callback.callback.c_str());
			if(iter != s_callbackUpdate.end())
				iter->second(entity);
		}
	}
} 