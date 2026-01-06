#include "pch.h"
#include "ConfigManager.h"

#include "System/Files/JSONParser.h"
#include "System/Files/XMLParser.h"

ConfigManager* ConfigManager::Get()
{
	GameData& gd = GameData::Get();
	ASSERT(gd.configs != nullptr, "Config manager has no been set up yet");
	return gd.configs;
}

using namespace rapidjson;

static void JSONReadData(const Value& doc_values, Settings& settings)
{
	for (Value::ConstMemberIterator itr = doc_values.MemberBegin(); itr != doc_values.MemberEnd(); ++itr)
	{
		if (itr->value.GetType() == kJsonType::Number)
		{
			settings.values[itr->name.GetString()] = itr->value.GetFloat();
		}
		else if (itr->value.GetType() == kJsonType::True)
		{
			settings.values[itr->name.GetString()] = true;
		}
		else if (itr->value.GetType() == kJsonType::False)
		{
			settings.values[itr->name.GetString()] = false;
		}
		else if (itr->value.GetType() == kJsonType::String)
		{
			settings.strings[itr->name.GetString()] = itr->value.GetString();
		}
	}
}

static void XMLReadStrings(const XMLNode& node, Settings& out_string)
{
	XMLNode childNode = node.child();
	while (childNode)
	{
		char* p = nullptr;
		const char* in_value = childNode.value();
		long converted = strtol(in_value, &p, 10);
		if (*p)
		{
			// conversion failed because the input wasn't a number
			bool is_true = strncmp(in_value, "true", strlen("true")) == 0;
			bool is_false = strncmp(in_value, "false", strlen("false")) == 0;
			if (is_true || is_false)
			{
				out_string.values[childNode.name()] = (int)is_true;
			}
			else
			{
				out_string.strings[childNode.name()] = childNode.value();
			}
		}
		else
		{
			out_string.values[childNode.name()] = converted;
		}

		childNode = childNode.next();
	}
}

void ConfigManager::ParseAll()
{
	std::vector<BasicString> configs;
	FileManager::Get()->GetFilesInFolder(FileManager::Configs, configs);

	for (u32 i = 0; i < configs.size(); i++)
	{
		bool success = Parse(configs[i].c_str());
		if (!success)
			DebugPrint(Warning, "Failed to load config %s", configs[i].c_str());
	}
}

bool ConfigManager::Parse(const char* path)
{
	bool did_read = false;

	if (FileManager::Get()->IsValidPath(path))
	{
		// check ext then run xml or json?
		JSONParser parser(path);

		if (!parser.document.IsObject())
			return false;

		if (parser.document.HasMember("types"))
		{
			Value& types = parser.document["types"];
			if (types.IsArray())
			{
				for (u32 i = 0; i < types.Size(); i++)
				{
					Value& type = types[i];

					ASSERT(type.HasMember("id"), "config has no id");

					const char* id = type["id"].GetString();
					mConfigs[id] = new Config(id);
					JSONReadData(type, mConfigs[id]->data);

					did_read = true;
				}
			}
		}
		else
		{
			ASSERT(parser.document.HasMember("id"), "config %s has no id", path);

			Config* config = new Config();
			JSONReadData(parser.document, config->data);
			config->name = parser.document["id"].GetString();

			mConfigs[config->name] = config;

			did_read = true;
		}
	}

	return did_read;
}

const Config* ConfigManager::GetConfig(const char* config)
{
	if (mConfigs.contains(config))
	{
		ASSERT(mConfigs.contains(config), "config %s has not been parsed yet, no data", config);
		return mConfigs.at(config);
	}
	else if(FileManager::Get()->exists(FileManager::Configs, config))
	{
		if(Parse(config))
			return mConfigs.at(config);
	}

	DebugPrint(Warning, "No config in the config manager with name: %s", config);
	return nullptr;
}