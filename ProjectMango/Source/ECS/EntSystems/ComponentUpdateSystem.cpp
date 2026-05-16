#include "pch.h"
#include "ComponentUpdateSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/IncludeComponents.h"
#include "Entities/Registries/StatusEffectRegistry.h"
#include "Game/States/GameState.h"
#include "Core/Helpers.h"

namespace ECS
{
	void ComponentUpdateSystem::Update(float dt)
	{
		std::vector<Entity> jigglers_to_remove;

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
				jigglers_to_remove.push_back(entity);
		}

		for (u32 i = 0; i < jigglers_to_remove.size(); i++)
		{
			if (Sprite* sprite = GetComponent(Sprite, jigglers_to_remove[i]))
			{
				sprite->params.renderOffset.x = 0.0f;
			}
			RemoveComponent(Jiggler, jigglers_to_remove[i]);
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

		int turn_index = GameState::GetTurnIndex();

		// StatusEffects
		ComponentArray<StatusEffects>& status_effects_comps =  GetAllComponents(StatusEffects);
		for( auto iter = status_effects_comps.entityToComponent.begin(); iter != status_effects_comps.entityToComponent.end(); iter++ )
		{
			StatusEffects& status_effects = status_effects_comps.GetComponentByIndex(iter->second);
			Entity entity = iter->first;

			for( int i = 0; i < (int)status_effects.effects.size(); i++ )
			{
				StatusEffect& effect = status_effects.effects[i];

				// trigger the on apply effect on first call
				if(!effect.onApplied)
				{
					StatusEffectRegistry::OnApply(effect, status_effects.entity);
					effect.onApplied = true;
				}

				// remove once duration has expired (relative to number of turns taken)
				if(effect.turnApplied + effect.turnDuration <= turn_index)
				{
					Erase(status_effects.effects, effect);
					--i;
				}
			}
		}

		// DeathScentence
		std::vector<Entity> entities_to_destroy;

		ComponentArray<DeathScentence>& death_scentences = GetAllComponents(DeathScentence);
		for (auto iter = death_scentences.entityToComponent.begin(); iter != death_scentences.entityToComponent.end(); iter++)
		{
			DeathScentence& death_scentence = death_scentences.GetComponentByIndex(iter->second);
			if (death_scentence.deathTimer != -FLT_MAX)
			{
				death_scentence.deathTimer -= dt;
			}

			if (!death_scentence.CanDie())
				continue;

			bool destroy_entity = false;

			// animator trigger
			if (death_scentence.deathLoops != -1)
			{
				bool animate_on_exit = false;

				const Animator* animator = GetComponent(Animator, iter->first);
				if (animator)
				{
					animate_on_exit = animator->GetAnimation(death_scentence.action) != nullptr;
				}

				if (animate_on_exit)
				{
					if (animator->GetActiveAnimation()->action == death_scentence.action)
					{
						if (animator->loopCount >= death_scentence.deathLoops)
						{
							destroy_entity = true;
						}
					}
				}
				else
				{
					destroy_entity = true;
				}
			}

			if (destroy_entity)
			{
				float fade_out_time = death_scentence.fadeOutTime;
				if (fade_out_time > 0.0f && fade_out_time > death_scentence.fadeOutTimer)
				{
					death_scentence.fadeOutTimer += dt;

					if (Sprite* sprite = GetComponent(Sprite, iter->first))
					{
						float progress = death_scentence.fadeOutTimer / fade_out_time;
						float alpha = 1.0f - Maths::EaseOutCubic(progress);

						sprite->params.colourMod.setOpacity(alpha);
					}
				}
				else
				{
					entities_to_destroy.push_back(iter->first);
				}
			}
		}

		for (u32 i = 0; i < entities_to_destroy.size(); i++)
		{
			ecs->entities.KillEntity(entities_to_destroy[i]);
		}
	}
}
	