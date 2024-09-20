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

static void ReadValues(const rapidjson::Value& doc_values, SettingValues& out_values)
{
	for (rapidjson::Value::ConstMemberIterator itr = doc_values.MemberBegin(); itr != doc_values.MemberEnd(); ++itr)
	{
		if(itr->value.GetType() == rapidjson::kJsonType::Number)
		{
			out_values[itr->name.GetString()] = itr->value.GetFloat();
		}
		else if(itr->value.GetType() == rapidjson::kJsonType::True)
		{
			out_values[itr->name.GetString()] = true;
		}
		else if(itr->value.GetType() == rapidjson::kJsonType::False)
		{
			out_values[itr->name.GetString()] = false;
		}
	}
}

void ObjectConfig::Read(const char* path)
{
	JSONParser parser(path);

	if (!parser.document.IsObject())
		return;

	// animation
	if(parser.document.HasMember("animation"))
		animation = parser.document["animation"].GetString();

	// spawn ID
	if (parser.document.HasMember("spawn_id"))
		spawnId = parser.document["spawn_id"].GetString();

	// fill all the values
	if (parser.document.HasMember("values"))
		ReadValues(parser.document["values"], values);

	parsed = true;
}
