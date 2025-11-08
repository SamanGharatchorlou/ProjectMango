#pragma once

#include "ECS/SystemManager.h"

struct RenderPack;

namespace ECS
{
	struct Sprite;

	class RenderSystem : public EntitySystem
	{
	public:
		RenderSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;
		
	};
	
	void GenerateRenderPack(const Sprite& sprite, RenderPack& pack);
}