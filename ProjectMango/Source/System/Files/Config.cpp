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

static void ReadStrings(const rapidjson::Value& doc_values, SettingStrings& out_string)
{
	for (rapidjson::Value::ConstMemberIterator itr = doc_values.MemberBegin(); itr != doc_values.MemberEnd(); ++itr)
	{
		if (itr->value.GetType() == rapidjson::kJsonType::String)
		{
			out_string[itr->name.GetString()] = itr->value.GetString();
		}
	}
}

void ObjectConfig::Read(const char* path)
{
	JSONParser parser(path);

	if (!parser.document.IsObject())
		return;

	//// animation
	//const char* animation = "animation";
	//if(parser.document.HasMember(animation))
	//	strings[animation] = parser.document[animation].GetString();

	//// spawn ID
	//const char* spawn_id = "spawn_id";
	//if (parser.document.HasMember(spawn_id))
	//	strings[spawn_id] = parser.document[spawn_id].GetString();

	ReadStrings(parser.document, strings);

	// fill all the values
	if (parser.document.HasMember("values"))
		ReadValues(parser.document["values"], values);

	parsed = true;
}
