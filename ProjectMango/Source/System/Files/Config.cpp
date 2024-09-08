#include "pch.h"
#include "Config.h"

#include "System/Files/JSONParser.h"

void GameSettingsConfig::Read(const char* path)
{
	XMLParser* parser = new XMLParser;

	parser->parseXML(path);
	settings.fillValues(parser->rootNode());

	parsed = true;

	delete parser;
}

void PlayerDataConfig::Read(const char* path)
{
	JSONParser parser(path);
	
	if(!parser.document.IsObject())
		return;

	animation = parser.document["animation"].GetString();

	// fill all the values
	const rapidjson::Value& values = parser.document["values"];
	for (rapidjson::Value::ConstMemberIterator itr = values.MemberBegin(); itr != values.MemberEnd(); ++itr)
	{
		if(itr->value.GetType() == rapidjson::kJsonType::Number)
		{
			settings[itr->name.GetString()] = itr->value.GetFloat();
		}
	}
	
	parsed = true;
}