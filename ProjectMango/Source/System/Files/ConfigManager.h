#pragma once

struct Config
{
	enum Type
	{
		JSON,
		XML
	};

	Config() { }
	Config(const char* config_name) : name(config_name) {};

	//void Read(const char* path);

	// dont want this settings data anymore
	// just keep the Document around in stead, easier and i dont have to reimplement all the types
	Settings data;

	StringBuffer32 name;
	//bool parsed = false;
	Type type = JSON;
};

class ConfigManager
{
public:
	static ConfigManager* Get();

	//void Add(const char* path, Config::Type type = Config::JSON);
	//Config* AddAndLoad(const char* path, Config::Type type = Config::JSON);
	
	//bool ValidPath(const char* path) const;

	//void Load();
	//void Reload();

	bool Parse(const char* path);
	void ParseAll();


	static void GetFullPath(const char* name, BasicString& out_path);

	const Config* GetConfig(const char* config);

	std::unordered_map<StringBuffer32, Config> mConfigs;
};

const Config* GetConfig(const char* config);