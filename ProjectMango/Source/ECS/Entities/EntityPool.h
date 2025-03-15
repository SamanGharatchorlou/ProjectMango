#pragma once

namespace ECS
{
	struct EntityMetaData;
	typedef Entity(*CreateEntityFn)(const EntityMetaData& emd);

	// dont both with this for now?
	struct EntityPool
	{
		Signature signature;

		std::vector< Entity > entities;
		
		void BuildEntities(CreateEntityFn createFn, EntityMetaData& data, u32 count);
	};
};
