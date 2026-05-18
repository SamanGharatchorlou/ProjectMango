#pragma once

#include "ECS/Components/ComponentHelpers.h"

typedef std::vector<ECS::EntityMetaData> UIScreenMetaData;
typedef std::vector<ECS::Entity> UIScreenEntities;

struct UIManager
{
	static UIManager& Get();
	
	// [ screen ID, meta data for screen ]
	std::unordered_map<BasicString, UIScreenMetaData> screenMetaData;

	// [ screen ID, entities for screen ]
	std::unordered_map<BasicString, UIScreenEntities> screenEntities;

	void OpenScreen(const char* screen_name);
	void CloseScreen(const char* screen_name);
	void RefreshScreen(const char* screen_name);
};