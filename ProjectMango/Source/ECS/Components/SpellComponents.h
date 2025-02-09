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
		ReboundRune(const char* config_id);

		void OnActiate(Entity entity) override;

		int reboundCount;
	};

	struct EchoRune : Rune
	{
		EchoRune(const char* config_id);

		void OnActiate(Entity entity) override;
		void Update(Entity entity) override;

		// required to re-create the spell
		VectorF target;
		Entity caster;
		int spellIndex;

		int echoCount;
		float echoTime;
		TimerF timer;
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
		void ActivateSpellToCursor(int spell_index);

		static Entity ActivateSpell(Entity caster, int spell_index, VectorF target);
	};

}