#pragma once

#include "Config.h"

class ConfigManager
{
public:
	static ConfigManager* Get();

	template<class T> 
	void add(const char* path)
	{
		if (mConfigs.count(path) == 0)
		{
			T* new_config = new T(path);
			mConfigs[path] = new_config;
		}
	}

	template<class T>
	T* AddAndLoad(const char* path)
	{
		if (mConfigs.count(path) == 0)
		{
			T* new_config = new T(path);
			mConfigs[path] = new_config;
		}

		Load();

		return static_cast<T*>(mConfigs[path]);
	}
	
	void Load();
	void Reload();

	static void GetFullPath(const char* name, BasicString& out_path);

	template<class T>
	static T* Parse(T& config)
	{
		BasicString full_path;
		GetFullPath(config.name.c_str(), full_path);

		if (full_path.empty())
			return nullptr;

		XMLParser* parser = new XMLParser;
		parser->parseXML(full_path.c_str());

		// todo: check the size of these, i was getting warnings about the parser being too
		// big for stack allocation, better to heap. buuuut, once i read it and turn it into useable
		// data im sure its fine? so here i am trying that, untested ofc...
		// well i pass it in, so i assumme thats a stacky one
		config.Read(*parser);
		config.parsed = true;

		delete parser;

	}

	template<class T>
	T* GetConfig(const char* config)
	{
		if (mConfigs.contains(config))
		{
			ASSERT(mConfigs[config]->parsed, "config %s has not been parsed yet, no data");
			return static_cast<T*>(mConfigs[config]);
		}
		else
		{
			return AddAndLoad<T>(config);
		}

		DebugPrint(Warning, "No config in the config manager with name: %s", config);
		return nullptr;
	}

private:
	std::unordered_map<StringBuffer32, Config*> mConfigs;
};