#pragma once

#include "Config.h"

struct Config;

class ConfigManager
{
public:
	static ConfigManager* Get();

	void Add(const char* path, Config::Type type = Config::JSON);
	Config* AddAndLoad(const char* path, Config::Type type = Config::JSON);
	
	//bool ValidPath(const char* path) const;

	void Load();
	void Reload();


	static void GetFullPath(const char* name, BasicString& out_path);

	//template<class T>
	//static T* Parse(T& config)
	//{
	//	BasicString full_path;
	//	GetFullPath(config.name.c_str(), full_path);

	//	if (full_path.empty())
	//		return nullptr;

	//	XMLParser* parser = new XMLParser;
	//	parser->parseXML(full_path.c_str());

	//	// todo: check the size of these, i was getting warnings about the parser being too
	//	// big for stack allocation, better to heap. buuuut, once i read it and turn it into useable
	//	// data im sure its fine? so here i am trying that, untested ofc...
	//	// well i pass it in, so i assumme thats a stacky one
	//	config.Read(*parser);
	//	config.parsed = true;

	//	delete parser;

	//}

	const Config* GetConfig(const char* config);

private:
	std::unordered_map<StringBuffer32, Config*> mConfigs;
};