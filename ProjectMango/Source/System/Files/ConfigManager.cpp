#include "pch.h"
#include "ConfigManager.h"

ConfigManager* ConfigManager::Get()
{
	GameData& gd = GameData::Get();
	ASSERT(gd.configs != nullptr, "Config manager has no been set up yet");
	return gd.configs;
}

void ConfigManager::GetFullPath(const char* name, BasicString& out_path)
{
	out_path = FileManager::Get()->findFile(FileManager::Configs, name);
	if (out_path.empty())
	{
		DebugPrint(Warning, "No config file found named %s found in config folder", name);
	}
}

void ConfigManager::Load()
{
	for (auto iter = mConfigs.begin(); iter != mConfigs.end(); iter++)
	{
		Config* config = iter->second;
		if (config->parsed)
			continue;

		BasicString file_path = FileManager::Get()->findFile(FileManager::Configs, iter->first.c_str());
		if(!file_path.empty())
		{
			config->Read(file_path.c_str());
			continue;
		}

		DebugPrint(Warning, "No config file found named %s found in config folder", iter->first.c_str());
	}
}

void ConfigManager::Add(const char* path, Config::Type type)
{
	if (mConfigs.count(path) == 0)
	{
		Config* new_config = new Config(path);
		mConfigs[path] = new_config;
		mConfigs[path]->type = type;
	}
}

Config* ConfigManager::AddAndLoad(const char* path, Config::Type type)
{
	Add(path, type);
	Load();
	return mConfigs[path];
}

void ConfigManager::Reload()
{
	for (auto iter = mConfigs.begin(); iter != mConfigs.end(); iter++)
	{
		iter->second->parsed = false;
	}

	Load();
}

const Config* ConfigManager::GetConfig(const char* config)
{
	if (mConfigs.contains(config))
	{
		ASSERT(mConfigs[config]->parsed, "config %s has not been parsed yet, no data", config);
		return mConfigs.at(config);
	}
	else if(FileManager::Get()->exists(FileManager::Configs, config))
	{
		return AddAndLoad(config);
	}

	DebugPrint(Warning, "No config in the config manager with name: %s", config);
	return nullptr;
}