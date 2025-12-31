#include "pch.h"
#include "MonsterRegistry.h"

#include "System/Files/JSONParser.h"

namespace MonsterRegistry
{
	std::vector<Monster> s_monsterRegistry;

	void Build(const char* file)
	{
		using namespace rapidjson;
		
		BasicString file_path;
		FileManager::Get()->FindFile(FileManager::Configs, file, file_path);
		JSONParser parser(file_path.c_str());
		if(!parser.IsValid())
			return;

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

		if(valid_indexes.size() == 0)
			return -1;

		int random_index = Maths::randomNumberBetween( 0, (int)valid_indexes.size());
		return valid_indexes[random_index];
	}
}