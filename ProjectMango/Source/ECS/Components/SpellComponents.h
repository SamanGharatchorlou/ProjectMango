#pragma once

namespace ECS
{
	struct SpellGem
	{
		virtual void OnActiate(Entity entity) { };
		virtual void Update(Entity entity) { }
	};

	struct SpellGem_Rebound : SpellGem
	{
		void OnActiate(Entity entity) override;
		void Update(Entity entity) override;

		int rebound_count;
	};

	struct Spell
	{
		COMPONENT_TYPE(Spell)

		BasicString name;
		SpellGem* gem;
	};

	struct SpellBook
	{
		COMPONENT_TYPE(SpellBook)

		~SpellBook();

		struct SpellSlot
		{
			BasicString name;
			SpellGem* gem;
		};

		static constexpr u32 c_spellCount = 3;

		SpellSlot spells[c_spellCount];

		void SetSpellSlot(int spell_index, const char* spell_name);
		void SetSpellSlotGem(int spell_index, SpellGem* spell_gem);

		bool CanActivateSpell(int spell_index) const;
		void ActivateSpell(int spell_index);
	};

}