#pragma once

struct Config
{
	Config(const char* config_name) : name(config_name) { };
	
	virtual void Read(const char* path) = 0;

	StringBuffer32 name;
	bool parsed = false;
};

typedef std::unordered_map<StringBuffer32, float> SettingValues;

struct GameSettingsConfig : public Config
{
	GameSettingsConfig(const char* config_name) : Config(config_name) { }

	void Read(const char* path) override;

	StringMap32 settings;
};

struct ObjectConfig : public Config
{
	ObjectConfig(const char* config_name) : Config(config_name) { }

	void Read(const char* path) override;

	BasicString animation;
	BasicString spawnId;
	SettingValues values;
};
