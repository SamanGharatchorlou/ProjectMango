#pragma once

struct Config
{
	Config(const char* config_name) : name(config_name) { };
	virtual void Read(XMLParser& parser) = 0;

	StringBuffer32 name;
	bool parsed = false;
};

struct GameSettingsConfig : public Config
{
	GameSettingsConfig(const char* config_name) : Config(config_name) { }
	void Read(XMLParser& parser) override;

	StringMap32 settings;
};


struct PopupConfig : public Config
{
	PopupConfig(const char* config_name) : Config(config_name) { }
	void Read(XMLParser& parser) override;

	BasicString title;
	BasicString body;
};