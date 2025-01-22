#include "pch.h"
#include "SpellComponents.h"

#include "Components.h"
#include "ECS/EntityCoordinator.h"
#include "Entities/Spells/SpellEntityBuilder.h"
#include "UIComponents.h"
#include "Collider.h"

namespace ECS
{
	// SpellBook
	// ------------------------------------------------------------------
	SpellBook::SpellBook()
	{
		for (u32 i = 0; i < c_spellCount; i++)
		{
			spells[i] = SpellSlot("", nullptr);
		}
	}

	SpellBook::~SpellBook()
	{
		for (u32 i = 0; i < c_spellCount; i++)
		{
			if(spells[i].rune)
				delete spells[i].rune;
		}
	}

	bool SpellBook::CanActivateSpell(int spell_index) const
	{
		if (spell_index < c_spellCount)
		{
			return Magic::SpellExists(spells[spell_index].name.c_str());
		}

		return false;
	}

	void SpellBook::ActivateSpell(int spell_index)
	{
		if (CanActivateSpell(spell_index))
		{
			EntityCoordinator* ecs = GameData::Get().ecs;
			ComponentArray<UICursor>& cursors = ecs->GetAllComponents(UICursor);
			if (cursors.Count() > 0)
			{
				auto front_index = cursors.entityToComponent.begin();
				UICursor& collider = cursors.GetComponentByIndex(front_index->second);

				Transform& transform = ecs->GetComponentRef(Transform, collider.entity);
				VectorF target = transform.GetRect().TopLeft();

				Entity spell_entity = Magic::GetNewEntity("Fireball", entity, target);

				Spell& spell = ecs->GetComponentRef(Spell, spell_entity);
				spell.rune = spells[spell_index].rune;

				if (spell.rune)
				{
					spell.rune->OnActiate(spell_entity);
				}
			}
		}
	}

	void SpellBook::SetSpellSlot(int spell_index, const char* spell_name)
	{
		if (spell_index < c_spellCount && Magic::SpellExists(spell_name) )
		{
			spells[spell_index].name.set(spell_name);
		}
	}

	void SpellBook::SetSpellSlotRune(int spell_index, Rune* rune)
	{
		if ( spell_index < c_spellCount )
		{
			// replace whatever we had before
			if (spells[spell_index].rune)
				delete spells[spell_index].rune;

			spells[spell_index].rune = rune;
		}
	}

	// SpellGem
	// ------------------------------------------------------------------
	Spell::Spell() : rune(nullptr) { }

	void RuneRebound::OnActiate(Entity entity)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		Collider& collider = ecs->GetComponentRef(Collider, entity);
		if (rebound_count > 0)
		{
			collider.destroyOnContact = false;
			collider.reboundCount = 3;
		}
	}

	void RuneRebound::Update(Entity entity)
	{
		//EntityCoordinator* ecs = GameData::Get().ecs;
		//Damage& damage = ecs->GetComponentRef(Damage, entity);
	}
}