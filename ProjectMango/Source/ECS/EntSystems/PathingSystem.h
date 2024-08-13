#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	static const VectorF c_tileSize = VectorF(4.0f, 4.0f);

	class PathingSystem : public EntitySystem
	{
	public:
		PathingSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;

		static std::vector<VectorI> FindPath(VectorF startPosition, VectorF endPosition);
	};
}