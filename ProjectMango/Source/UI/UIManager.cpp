#include "pch.h"
#include "UIManager.h"

#include "Entities/Factory/EntityBuilder.h"
#include "ECS/EntityCoordinator.h"
#include "Entities/Factory/ComponentAssembler.h"
#include "Entities/Factory/EntitySerialiser.h"

UIManager& UIManager::Get()
{
	GameData& gd = GameData::Get();
	ASSERT(gd.uiManager != nullptr, "Texture manager has no been set up yet");
	return *gd.uiManager;
}

void UIManager::Init()
{
	std::vector<BasicString> files;
	FileManager::Get()->GetFilesInFolder(FileManager::SaveData, files);
	for (u32 i = 0; i < files.size(); i++)
	{
		if (FileManager::HasExt(files[i].c_str(), ".uiscreen"))
		{
			StringBuffer64 file_name = FileManager::getItemName(files[i].c_str());

			UIScreenMetaData& meta_datas = screenMetaData[file_name.c_str()];
			LoadMetaDataFromJson(files[i].c_str(), meta_datas);
		}
	}

	initialised = true;
}


void UIManager::ShutDown()
{
	// clean up any entities
	for (auto& [key, val] : screenEntities)
	{
		CloseScreen(key.c_str());
	}

	screenEntities.clear();
	screenMetaData.clear();
	initialised = false;
}


void UIManager::CloseAllScreens()
{
	for (auto& [key, val] : screenEntities)
	{
		CloseScreen(key.c_str());
	}
}

void UIManager::OpenScreen(const char* screen_name)
{
	if (!initialised)
		Init();

	if (IsScreenOpen(screen_name))
	{
		CloseScreen(screen_name);
	}

	if (screenMetaData.contains(screen_name))
	{
		UIScreenMetaData& meta_datas = screenMetaData[screen_name];
		for (u32 i = 0; i < meta_datas.size(); i++)
		{
			const EntityMetaData& emd = meta_datas[i];

			ECS::Entity entity = AssembleEntity(emd);
			screenEntities[screen_name].push_back(entity);
		}

		// we need all the entities to exist at this point so we need to do it after
		for (u32 i = 0; i < meta_datas.size(); i++)
		{
			const EntityMetaData& emd = meta_datas[i];
			SetParentsAndChildren(emd);
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

bool UIManager::IsScreenOpen(const char* screen_name)
{
	if (screenEntities.contains(screen_name))
	{
		return screenEntities[screen_name].size() > 0;
	}

	return false;
}

void UIManager::RefreshScreen(const char* screen_name)
{
	CloseScreen(screen_name);
	OpenScreen(screen_name);
}