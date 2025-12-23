#include "pch.h"
#include "MonsterRegistry.h"

#include "System/Files/JSONParser.h"

namespace MonsterRegistry
{
	std::vector<Monster> s_monsterRegistry;

	void Build(const char* file)
	{
		using namespace rapidjson;

		BasicString full_path = FileManager::Get()->findFile(FileManager::Configs, file);
		if(full_path.length() == 0)
		{
			DebugPrint(PriorityLevel::Log, "Registry file does not exist: '%s'", file);
			return;
		}

		JSONParser parser(full_path.c_str());
		
		if(!parser.document.IsObject())
		{
			DebugPrint(PriorityLevel::Warning, "Invalid registry document: %s", full_path.c_str());
			return;
		}

		if(parser.document.HasMember("Monsters"))
		{
			if(parser.document["Monsters"].IsArray())
			{
				const Value::Array& monsters = parser.document["Monsters"].GetArray();

				s_monsterRegistry.resize(monsters.Size());

				for( u32 i = 0; i < monsters.Size(); i++ )
				{
					const Value& value = monsters[i];
					Monster& monster = s_monsterRegistry[i];
					monster.id = value["id"].GetString();
					monster.points = value["points"].GetInt();
					monster.registryIndex = i;
				}
			}
		}
	}

	
	const char* GetMonster(int index)
	{
		return s_monsterRegistry[index].id.c_str();
	}
	
	const char* GetRandomMonster(int points)
	{
		int random_index = GetRandomMonsterIndex(points);

		if(random_index != -1)
			return s_monsterRegistry[random_index].id.c_str();

		return nullptr;
	}

	
	int GetRandomMonsterIndex(int points)
	{
		if(points == 0)
			return -1;

		std::vector<int> valid_indexes;
		for( u32 i = 0; i < s_monsterRegistry.size(); i++ )
		{
			if(s_monsterRegistry[i].points == points)
				valid_indexes.push_back(i);
		}

		int random_index = Maths::randomNumberBetween( 0, (int)valid_indexes.size());
		return valid_indexes[random_index];
	}
}