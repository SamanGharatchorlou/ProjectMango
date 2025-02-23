#pragma once

struct Config
{
	enum Type
	{
		JSON,
		XML
	};

	Config(const char* config_name) : name(config_name) { };
	
	void Read(const char* path);
	
	Settings values;

	StringBuffer32 name;
	bool parsed = false;
	Type type = JSON;
};
