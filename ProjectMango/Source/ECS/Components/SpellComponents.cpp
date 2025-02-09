#include "pch.h"
#include "SpellComponents.h"

#include "Components.h"
#include "ECS/EntityCoordinator.h"
#include "Entities/Spells/SpellEntityBuilder.h"
#include "UIComponents.h"
#include "Collider.h"
#include "System/Files/Config.h"
#include "System/Files/ConfigManager.h"


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

	void SpellBook::ActivateSpellToCursor(int spell_index)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		ComponentArray<UICursor>& cursors = ecs->GetAllComponents(UICursor);
		if (cursors.Count() > 0)
		{
			auto front_index = cursors.entityToComponent.begin();
			UICursor& cursor = cursors.GetComponentByIndex(front_index->second);
			Transform& transform = ecs->GetComponentRef(Transform, cursor.entity);
			VectorF target = transform.GetRect().TopLeft();

			Entity spell_entity = ActivateSpell(entity, spell_index, target);

			// activate run rune
			Spell& spell = ecs->GetComponentRef(Spell, spell_entity);
			spell.rune = spells[spell_index].rune;
			if (spell.rune)
			{
				spell.rune->OnActiate(spell_entity);
			}
		}
	}

	Entity SpellBook::ActivateSpell(Entity caster, int spell_index, VectorF target)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		SpellBook& spell_book = ecs->GetComponentRef(SpellBook, caster);

		return Magic::GetNewEntity(spell_book.spells[spell_index].name.c_str(), caster, target);
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


	// ReboundRune
	// ------------------------------------------------------------------
	ReboundRune::ReboundRune(const char* config_id)
	{
		if(const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>(config_id))
		{
			if(config->values.Contains("rebound_count"))
			{	
				reboundCount = (int)config->values["rebound_count"];
			}
		}
	}

	void ReboundRune::OnActiate(Entity entity)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		Collider& collider = ecs->GetComponentRef(Collider, entity);
		if (reboundCount > 0)
		{
			collider.destroyOnContact = false;
			collider.reboundCount = reboundCount;
		}
	}

	// EchoRune
	// ------------------------------------------------------------------
	EchoRune::EchoRune(const char* config_id)
	{
		if (const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>(config_id))
		{
			if (config->values.Contains("echo_count"))
			{
				echoCount = (int)config->values["echo_count"];
			}

			if (config->values.Contains("echo_time"))
			{
				echoCount = (int)config->values["echo_time"];
			}
		}
	}

	void EchoRune::OnActiate(Entity entity)
	{
		timer.Start();
	}

	void EchoRune::Update(Entity entity)
	{
		if (echoCount > 0 && timer.GetSeconds() >= echoTime)
		{
			echoCount--;
			Entity echo_spell = SpellBook::ActivateSpell(caster, spellIndex, target);
		}
	}
}