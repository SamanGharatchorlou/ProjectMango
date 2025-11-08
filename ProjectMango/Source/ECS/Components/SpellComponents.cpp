#include "pch.h"
#include "SpellComponents.h"

#include "Components.h"
#include "ECS/EntityCoordinator.h"
#include "Entities/Weapons/SpellEntityBuilder.h"
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
		ComponentArray<UICursor>& cursors = GetAllComponents(UICursor);
		if (cursors.Count() > 0)
		{
			auto front_index = cursors.entityToComponent.begin();
			UICursor& cursor = cursors.GetComponentByIndex(front_index->second);
			Transform& transform = GetComponentRef(Transform, cursor.entity);
			VectorF target = transform.GetRect().TopLeft();

			Entity spell_entity = ActivateSpell(entity, spell_index, target);

			// activate rune
			Spell& spell = GetComponentRef(Spell, spell_entity);
			//spell.rune = spells[spell_index].rune;
			if (spells[spell_index].rune)
			{
				spells[spell_index].rune->OnActiate(spell_entity);
			} 
		}
	}

	Entity SpellBook::ActivateSpell(Entity caster, int spell_index, VectorF target)
	{
		SpellBook& spell_book = GetComponentRef(SpellBook, caster);

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
			spells[spell_index].rune->caster = entity;
			spells[spell_index].rune->spellIndex = spell_index;
		}
	}

	// SpellGem
	// ------------------------------------------------------------------
	Spell::Spell() { }


	// ReboundRune
	// ------------------------------------------------------------------
	ReboundRune::ReboundRune(const char* id, const char* config) : Rune(id, config)
	{
		if(const Config* obj_config = ConfigManager::Get()->GetConfig(id))
		{
			reboundCount = obj_config->data.GetInt("rebound_count");
		}
	}

	void ReboundRune::OnActiate(Entity entity)
	{
		Collider& collider = GetComponentRef(Collider, entity);
		if (reboundCount > 0)
		{
			collider.destroyOnContact = false;
			collider.reboundCount = reboundCount;
		}
	}

	// EchoRune
	// ------------------------------------------------------------------
	EchoRune::EchoRune(const char* id, const char* config) : 
		Rune(id, config),
		echoCount(0), echoTime(0)
	{
		if (const Config* obj_config = ConfigManager::Get()->GetConfig(id))
		{
			echoCount = obj_config->data.GetInt("echo_count");
			echoTime = obj_config->data.GetFloat("echo_time");
		}
	}

	void EchoRune::OnActiate(Entity entity)
	{
		Instance echo;
		echo.timer.Start();

		const Spell& spell = GetComponentRef(Spell, entity);

		echo.target = spell.target;
		echo.count = echoCount;

		echos.push(echo);
	}

	void EchoRune::Update()
	{
		if (echos.size() > 0)
		{
			Instance& echo = echos.front();

			//float secs = echo.timer.GetSeconds();
			if (echo.timer.GetSeconds() >= echoTime)
			{
				echo.count--;
				Entity entity = SpellBook::ActivateSpell(caster, spellIndex, echo.target);

				Sprite& sprite = GetComponentRef(Sprite, entity);

				sprite.colourMod = SColour(SColour::Blue);
				sprite.colourMod.a = (Uint8)(c_alphaMax * 0.75f);
			}

			if (echo.count <= 0)
				echos.popFront();
		}
	}
}