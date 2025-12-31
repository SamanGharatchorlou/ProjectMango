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
		const char* id = itr->name.GetString();
		switch( itr->value.GetType() )
		{	
			case kJsonType::Number:
			{
				settings.values[id] = itr->value.GetFloat();
				break;
			}
			case kJsonType::True:
			{
				settings.values[id] = true;
				break;
			}
			case kJsonType::False:
			{
				settings.values[id] = false;
				break;
			}
			case kJsonType::String:
			{
				settings.strings[id] = itr->value.GetString();
				break;
			}
			case kJsonType::Array:
			{			
				const rapidjson::Value& value = itr->value;

				const Value::ConstArray& array = value.GetArray();
			
				if(array.Size() > 0)
				{
					if(array.begin()->GetType() == kJsonType::Number)
					{
						std::vector<float>& float_array = settings.floatArrays.data[id];
						for( u32 i = 0; i < array.Size(); i++ )
						{
							float_array.push_back(array[i].GetFloat());
						}
					}

				}
				break;
			}
			default:
				break;
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
				out_string.values[childNode.name()] = (float)((int)is_true);
			}
			else
			{
				out_string.strings[childNode.name()] = childNode.value();
			}
		}
		else
		{
			out_string.values[childNode.name()] = (float)converted;
		}

		childNode = childNode.next();
	}
}

void ConfigManager::GetFullPath(const char* name, BasicString& out_path)
{
	if (!FileManager::Get()->FindFile(FileManager::Configs, name, out_path))
	{
		DebugPrint(Warning, "No config file found named %s found in config folder", name);
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

bool ConfigManager::Parse(const char* full_path)
{
	bool did_read = false;

	FileManager* fm = FileManager::Get();
	if (fm->IsValidPath(full_path))
	{
		// if its an animation we read that somewhere else, no need to do it here
		if(!fm->IsFileInFolder(FileManager::Config_Data, full_path))
			return true;

		// check ext then run xml or json?
		JSONParser parser(full_path);
		if (!parser.IsValid())
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
					mConfigs[id] = Config(id);
					JSONReadData(type, mConfigs[id].data);

					did_read = true;
				}
			}
		}
		else
		{
			ASSERT(parser.document.HasMember("id"), "config %s has no id", full_path);

			Config config;
			JSONReadData(parser.document, config.data);
			config.name = parser.document["id"].GetString();

			mConfigs[config.name] = config;

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
		return &mConfigs.at(config);
	}
	else if(FileManager::Get()->exists(FileManager::Configs, config))
	{
		BasicString full_path;
		FileManager::Get()->FindFile(FileManager::Configs, config, full_path);
		if(Parse(full_path.c_str()))
			return &mConfigs.at(config);
	}

	return nullptr;
}

const Config* GetConfig(const char* config)
{
	return ConfigManager::Get()->GetConfig(config);
}