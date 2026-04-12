#pragma once

struct Monster
{
	BasicString id;
	int points = 0;
	int registryIndex = 0;
};

namespace MonsterRegistry
{
	void Build(const char* config);
	void Clear();

	const char* GetMonster(int index);
	const char* GetRandomMonster(int points);
	int GetRandomMonsterIndex(int points);
}