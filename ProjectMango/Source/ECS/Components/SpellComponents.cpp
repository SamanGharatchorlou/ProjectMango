#include "pch.h"
#include "SpellComponents.h"

#include "Components.h"
#include "ECS/EntityCoordinator.h"
#include "Entities/Spells/SpellEntityBuilder.h"
#include "UIComponents.h"

namespace ECS
{
	// SpellBook
	// ------------------------------------------------------------------
	SpellBook::SpellBook()
	{
		for (u32 i = 0; i < c_spellCount; i++)
		{
			spells[i] = SpellSlot(BasicString(), nullptr);
		}
	}

	SpellBook::~SpellBook()
	{
		for (u32 i = 0; i < c_spellCount; i++)
		{
			if(spells[i].gem)
				delete spells[i].gem;

			if(!spells[i].name.empty())
			{
				delete[] spells[i].name.buffer();
				spells[i].name.eliminate();
			}
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
				spell.gem = spells[spell_index].gem;

				if (spell.gem)
				{
					spell.gem->OnActiate(spell_entity);
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

	void SpellBook::SetSpellSlotGem(int spell_index, SpellGem* spell_gem)
	{
		if ( spell_index < c_spellCount )
		{
			if (spells[spell_index].gem)
				delete spells[spell_index].gem;


			spells[spell_index].gem = spell_gem;
		}
	}

	// SpellGem
	// ------------------------------------------------------------------
	Spell::Spell() : gem(nullptr) { }

	void SpellGem_Rebound::OnActiate(Entity entity)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		//Collider& collider = ecs->GetComponentRef(Collider, entity);
		//if (rebound_count > 0)
		//{
		//	damage.destroyOnContact = false;
		//}
	}

	void SpellGem_Rebound::Update(Entity entity)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		//Damage& damage = ecs->GetComponentRef(Damage, entity);
	}
}