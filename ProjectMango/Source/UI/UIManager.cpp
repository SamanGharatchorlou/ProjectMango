#include "pch.h"
#include "UIManager.h"

#include "Entities/Factory/EntityBuilder.h"
#include "ECS/EntityCoordinator.h"
#include "Entities/Factory/ComponentAssembler.h"

UIManager& UIManager::Get()
{
	static UIManager s_uiManager;
	return s_uiManager;
}

void UIManager::OpenScreen(const char* screen_name)
{
	//if(screenMetaData.contains(screen_name))
	//	CreateEntitiesFromData(screenMetaData[screen_name], screenEntities[screen_name]);

	if (screenMetaData.contains(screen_name))
	{
		UIScreenMetaData& meta_datas = screenMetaData[screen_name];
		for (u32 i = 0; i < meta_datas.size(); i++)
		{
			const EntityMetaData& emd = meta_datas[i];

			ECS::Entity entity = AssembleEntity(emd);
			screenEntities[screen_name].push_back(entity);
		}
	}
}

void UIManager::CloseScreen(const char* screen_name)
{
	if (screenEntities.contains(screen_name))
	{
		UIScreenEntities& entities = screenEntities[screen_name];
		for (u32 i = 0; i < entities.size(); i++)
		{
			ecs->entities.KillEntity(entities[i]);
		}

		entities.clear();
	}
}

void UIManager::RefreshScreen(const char* screen_name)
{
	CloseScreen(screen_name);
	OpenScreen(screen_name);
}