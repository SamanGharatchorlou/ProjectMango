#include "pch.h"
#include "ComponentUpdateSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/GraphicComponents.h"

namespace ECS
{
	void ComponentUpdateSystem::Update(float dt)
	{
		std::vector<Entity> entities_to_destroy;

		for (Entity entity : entities)
		{		
			if(Jiggler* jiggler = GetComponent(Jiggler, entity) )
			{
				if(jiggler->startTime == 0)
				{
					jiggler->startTime = GetTicksMS();
					if(Maths::randomNumberBetween(0,2) == 1)
						jiggler->amplitude = -jiggler->amplitude;
				}

				u64 time = GetTicksMS() - jiggler->startTime;
				float time_since_start = (float)(time) / 1000.0f;

				float t = jiggler->frequency * time_since_start;
				if(jiggler->decayStartTime == 0 && t > (2.0 * M_PI * (double)jiggler->undisturbedLoops))
				{
					jiggler->decayStartTime = GetTicksMS();
				}

				float decay = 1.0f;
				if(jiggler->decayStartTime > 0)
				{
					u64 decay_time = GetTicksMS() - jiggler->decayStartTime;
					float time_of_decay = (float)(decay_time) / 1000.0f;
					decay = expf(-jiggler->decayTime * time_of_decay);
				}

				if(Sprite* sprite = GetComponent(Sprite, entity))
				{
					float t = jiggler->frequency * time_since_start;
					sprite->params.renderOffset.x = jiggler->amplitude * sinf(t) * decay;
				}

				if(decay < 0.1f)
					entities_to_destroy.push_back(entity);
			}
		}

		for( u32 i = 0; i < entities_to_destroy.size(); i++ )
		{			
			if(Sprite* sprite = GetComponent(Sprite, entities_to_destroy[i]))
			{
				sprite->params.renderOffset.x = 0.0f;
			}
			RemoveComponent(Jiggler, entities_to_destroy[i]);	
		}

		entities_to_destroy.clear();
	}
}
	