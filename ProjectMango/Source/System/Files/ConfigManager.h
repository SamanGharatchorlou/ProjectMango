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

	Settings data;

	StringBuffer32 name;
	Type type = JSON;
};

struct AnimationConfig : public Config
{
	struct Spritesheet
	{
		BasicString id;
		std::vector<Settings> animations;
	};

	std::vector<Spritesheet> spriteSheets;
};

class ConfigManager
{
public:
	static ConfigManager* Get();

	bool Parse(const char* path);
	void ParseAll();

	const Config* GetConfig(const char* config);

	std::unordered_map<StringBuffer32, Config*> mConfigs;
};