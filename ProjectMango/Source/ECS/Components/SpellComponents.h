#pragma once

namespace ECS
{
	struct Rune
	{
		virtual void OnActiate(Entity entity) { };
		virtual void Update(Entity entity) { }
	};

	struct ReboundRune : Rune
	{
		void OnActiate(Entity entity) override;
		void Update(Entity entity) override;

		int rebound_count;
	};

	struct Spell
	{
		COMPONENT_TYPE(Spell)

		BasicString name;
		Rune* rune;
	};

	struct SpellBook
	{
		COMPONENT_TYPE(SpellBook)

		static constexpr u32 c_spellCount = 3;

		struct SpellSlot
		{
			StringBuffer64 name;
			Rune* rune;
		};

		SpellSlot spells[c_spellCount];

		~SpellBook();

		void SetSpellSlot(int spell_index, const char* spell_name);
		void SetSpellSlotRune(int spell_index, Rune* rune);

		bool CanActivateSpell(int spell_index) const;
		void ActivateSpell(int spell_index);
	};

}