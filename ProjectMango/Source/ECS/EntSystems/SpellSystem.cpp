#include "pch.h"
#include "SpellSystem.h"

#include "ECS/EntityCoordinator.h"
#include "Entities/Spells/SpellEntityBuilder.h"
#include "ECS/Components/SpellComponents.h"
#include "ECS/Components/Animator.h"
#include "Input/InputManager.h"

namespace ECS
{
	void SpellSystem::Update(float dt)
	{
		// setup spell - where to put this?
		Magic::CreateEntityMap();

		for (u32 i = 0; i < entitiesToDestroy.size(); i++)
		{
			ecs->entities.KillEntity(entitiesToDestroy[i]);
		}

		for (Entity entity : entities)
		{
			// SpellBooks
			if (SpellBook* spell_book = GetComponent(SpellBook, entity))
			{
				for (u32 i = 0; i < SpellBook::c_spellCount; i++)
				{
					if (spell_book->spells[i].rune)
					{
						spell_book->spells[i].rune->Update();
					}
				}

				InputManager* input = InputManager::Get();
				if (input->isPressed(Button::One, c_inputBuffer))
				{
					spell_book->SetSpellSlot(0, "Fireball");
				}
				if (input->isPressed(Button::Two, c_inputBuffer))
				{
					spell_book->SetSpellSlot(0, "Lightning");
				}
			}

			// Spells
			if (Spell* spell = GetComponent(Spell, entity))
			{
				if (Animator* animator = GetComponent(Animator, entity))
				{
					if (!animator->GetActiveAnimation().looping && animator->loopCount > 0)
					{
						entitiesToDestroy.push_back(entity);
					}
				}
			}
		}
	}
}