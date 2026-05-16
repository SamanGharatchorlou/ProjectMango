#include "pch.h"
#include "UIManager.h"

#include "Entities/EntityBuilder.h"

UIManager& UIManager::Get()
{
	static UIManager s_uiManager;
	return s_uiManager;
}

void UIManager::OpenScreen(const char* screen_name)
{
	if(screenMetaData.contains(screen_name))
		CreateEntitiesFromData(screenMetaData[screen_name], screenEntities[screen_name]);
}