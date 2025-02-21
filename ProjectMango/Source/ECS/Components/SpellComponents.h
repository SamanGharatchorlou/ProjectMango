#pragma once

namespace ECS
{
	struct Rune
	{
		Rune(const char* _id, const char* _config) : id(_id), config(_config), caster(EntityInvalid), spellIndex(0) { }
		virtual void OnActiate(Entity entity) { };
		virtual void Update() { }

		BasicString id;
		BasicString config;

		// put these in the update parameter?
		Entity caster;
		int spellIndex;
	};

	struct ReboundRune : Rune
	{
		ReboundRune(const char* id, const char* config);

		void OnActiate(Entity entity) override;

		int reboundCount;
	};

	struct EchoRune : Rune
	{
		EchoRune(const char* id, const char* config);

		void OnActiate(Entity entity) override;
		void Update() override;

		// number of echos
		int echoCount;
		float echoTime;

		// one instance of an echo, we might have multiple at a time
		struct Instance
		{
			VectorF target;
			TimerF timer;
			int count = 0;
		};

		Queue<Instance> echos;
	};

	struct Spell
	{
		COMPONENT_TYPE(Spell)

		BasicString name;
		//Rune* rune;

		// original target
		VectorF target;

		Entity caster;
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