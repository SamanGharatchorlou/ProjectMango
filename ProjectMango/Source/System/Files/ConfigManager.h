#pragma once

struct Config
{
	enum Type
	{
		JSON,
		XML
	};

	Config() { }
	Config(const char* config_name) /*: name(config_name)*/ {};

	Settings data;

	//StringBuffer32 name;
	Type type = JSON;
};

struct ConfigManager
{
public:
	static ConfigManager* Get();

	bool Parse(const char* path);
	void ParseAll();

	static void GetFullPath(const char* name, BasicString& out_path);

	const Config* GetConfig(const char* config);

	std::unordered_map<StringBuffer32, Config> mConfigs;
};

const Config* GetConfig(const char* config);