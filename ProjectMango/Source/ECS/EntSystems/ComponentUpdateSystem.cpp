#include "pch.h"
#include "ComponentUpdateSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/IncludeComponents.h"

namespace ECS
{
	void ComponentUpdateSystem::Update(float dt)
	{
		std::vector<Entity> entities_to_destroy;

		// Jiggler
		ComponentArray<Jiggler>& jigglers =  GetAllComponents(Jiggler);
		for( auto iter = jigglers.entityToComponent.begin(); iter != jigglers.entityToComponent.end(); iter++ )
		{
			Jiggler& jiggler = jigglers.GetComponentByIndex(iter->second);
			Entity entity = iter->first;

			if(jiggler.startTime == 0)
			{
				jiggler.startTime = GetTicksMS();
				if(Maths::randomNumberBetween(0,2) == 1)
					jiggler.amplitude = -jiggler.amplitude;
			}

			u64 time = GetTicksMS() - jiggler.startTime;
			float time_since_start = (float)(time) / 1000.0f;

			float t = jiggler.frequency * time_since_start;
			if(jiggler.decayStartTime == 0 && t > (2.0 * M_PI * (double)jiggler.undisturbedLoops))
			{
				jiggler.decayStartTime = GetTicksMS();
			}

			float decay = 1.0f;
			if(jiggler.decayStartTime > 0)
			{
				u64 decay_time = GetTicksMS() - jiggler.decayStartTime;
				float time_of_decay = (float)(decay_time) / 1000.0f;
				decay = expf(-jiggler.decayTime * time_of_decay);
			}

			if(Sprite* sprite = GetComponent(Sprite, entity))
			{
				float t = jiggler.frequency * time_since_start;
				sprite->params.renderOffset.x = jiggler.amplitude * sinf(t) * decay;
			}

			if(decay < 0.1f)
				entities_to_destroy.push_back(entity);
		}

		// UIIntentIcon
		ComponentArray<UIIntentIcon>& intent_icons =  GetAllComponents(UIIntentIcon);
		for( auto iter = intent_icons.entityToComponent.begin(); iter != intent_icons.entityToComponent.end(); iter++ )
		{
			UIIntentIcon& intent_icon = intent_icons.GetComponentByIndex(iter->second);
			Entity entity = iter->first;
			
			Sprite& child_sprite = GetComponentRef(Sprite, entity);

			// clear the texture first, we will reset it if we find a valid intent
			child_sprite.SetTexture(nullptr);

			Entity parent = GetParent(entity);
			if(const AIStrategy* strategy = GetComponent(AIStrategy, parent))
			{
				// show what its going to do next, unless its doing the thing in which case show the thing
				if(const TurnState* turn = GetComponent(TurnState, parent))
				{
					EnemyPhase::Type next_phase = strategy->GetCurrentPhase().type;
					for( u32 i = 0; i < intent_icon.displays.size(); i++ )
					{
						if(intent_icon.displays[i].intent == next_phase)
						{
							child_sprite.SetTexture(intent_icon.displays[i].icon.c_str());
						}
					}
				}
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
	