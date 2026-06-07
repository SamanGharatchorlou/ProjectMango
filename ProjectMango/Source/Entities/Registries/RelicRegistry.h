#pragma once

namespace ECS
{
	struct Relic;
}

namespace RelicRegistry
{
	void PopulateRegistry();
	ECS::Relic* GetRandomUnobtainedRelic();
	const std::vector<ECS::Relic>& GetRelicRegistry();
}