#pragma once

namespace ECS
{
	struct Relic;
}

namespace RelicRegistry
{
	void PopulateRegistry();
	ECS::Relic* GetRelic(const char* relic_id);
}