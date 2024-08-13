#include "pch.h"
#include "ConfigManager.h"

#include "Config.h"


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

//template<class T>
//T* ConfigManager::Parse(T& config)
//{
//	BasicString full_path;
//	GetFullPath(config.name, full_path);
//
//	if (full_path.empty())
//		return nullptr;
//
//	XMLParser* parser = new XMLParser;
//	parser->parseXML(full_path.c_str());
//
//	// todo: check the size of these, i was getting warnings about the parser being too
//	// big for stack allocation, better to heap. buuuut, once i read it and turn it into useable
//	// data im sure its fine? so here i am trying that, untested ofc...
//	// well i pass it in, so i assumme thats a stacky one
//	config->Read(*parser);
//	config->parsed = true;
//
//	delete parser;
//}

void ConfigManager::load()
{
	XMLParser* parser = nullptr;

	for (auto iter = mConfigs.begin(); iter != mConfigs.end(); iter++)
	{
		Config* config = iter->second;
		if (config->parsed)
			continue;

		if(!parser)
			parser = new XMLParser;

		BasicString full_path;
		GetFullPath(iter->first.c_str(), full_path);
		parser->parseXML(full_path.c_str());

		config->Read(*parser);
		config->parsed = true;
	}

	delete parser;
}

