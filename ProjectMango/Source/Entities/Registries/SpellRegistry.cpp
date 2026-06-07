#include "pch.h"
#include "SpellRegistry.h"

#include "System/Files/JSONParser.h"

#include "Entities/Factory/EntityBuilder.h"
#include "Entities/Factory/EntitySerialiser.h"
#include "ECS/Components/IncludeComponents.h"
#include "Game/Readers/AnimationReader.h"
#include "ECS/EntityCoordinator.h"
#include "Game/Camera/Camera.h"

namespace SpellRegistry
{
	struct CardSpell
	{
		// multiple of frame size
		float size = 1.0f;
		Colour::Type colour = Colour::Count;

		// spells have a range of damage that they can be assigned to
		int damageMin = 0;
		int damageMax = 0;

		bool operator == (const CardSpell& cs) const { 
			return colour == cs.colour &&
				damageMin == cs.damageMin && damageMax && cs.damageMax; 
		}
	};

	std::unordered_map<BasicString, CardSpell> s_spellsRegistry;

	static bool SpellExists(CardSpell& cs)
	{
		for (auto iter = s_spellsRegistry.begin(); iter != s_spellsRegistry.end(); iter++)
		{
			if (iter->second == cs)
				return true;
		}

		return false;
	}

	void Build(const char* file)
	{
		using namespace rapidjson;

		BasicString file_path;
		bool found = FileManager::Get()->FindFile(FileManager::Configs, file, file_path);
		if (found)
		{
			JSONParser parser(file_path.c_str());
			if (parser.IsValid())
			{
				const char* header = "Spells";
				if (parser.document.HasMember(header))
				{
					if (parser.document[header].IsArray())
					{
						const Value::Array& entries = parser.document[header].GetArray();

						s_spellsRegistry.reserve(entries.Size());

						for (u32 i = 0; i < entries.Size(); i++)
						{
							const Value& value = entries[i];

							CardSpell spell;
							//spell.damage = value["damage"].GetInt();
							spell.size = value["size"].GetFloat();
							spell.colour = Colour::s_stringToType.at(value["colour"].GetString());

							s_spellsRegistry[value["id"].GetString()] = spell;
						}
					}
				}
			}
		}

		// default populate any non-existing entries
		for (u32 i = 0; i < Colour::Count; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				CardSpell spell;
				spell.damageMin = j * 10;
				spell.damageMax = spell.damageMin + 10;
				spell.colour = (Colour::Type)i;

				if (SpellExists(spell))
					continue;

				char id_buffer[32];
				snprintf(id_buffer, 32, "%s_spell_%d", Colour::s_typeToString.at(spell.colour).c_str(), j + 1);

				s_spellsRegistry[id_buffer] = spell;
			}
		}
	}

	bool GetSpellMetaData(const char* spell_id, ECS::EntityMetaData& emd)
	{
		if (!spell_id || !AnimationReader::AnimationExists(spell_id))
			return false;

		const CardSpell& spell = s_spellsRegistry.at(spell_id);

		VectorF size = AnimationReader::GetAnimationFrameSize(spell_id);
		size = size * spell.size;

		PopulateMetaData(spell_id, RectF(VectorF::zero(), size), emd);
		//emd.data.AddInt("damage", spell.damage);
		emd.data.AddInt("colour", (int)spell.colour);

		return true;
	}

	Entity CreateSpell(const char* spell_id, int spell_damage, ECS::Entity target)
	{
		EntityMetaData meta_data;
		bool exists = GetSpellMetaData(spell_id, meta_data);
		if (!exists)
			return EntityInvalid;

		VectorF size = AnimationReader::GetAnimationFrameSize(spell_id);
		float tl_x = GetPosition(target).x - size.x * 0.5f;
		float tl_y = GetRect(target).BotPoint() - size.y;
		meta_data.data.AddVectorF("position", VectorF(tl_x, tl_y));

		Entity entity = CreateBasicObject(meta_data);

		Action::Enum action = Action::Active;

		// Animation
		Animator& animator = AddComponent(Animator, entity);
		animator.Init();
		animator.StartAnimation(action);

		// DeathScentence
		DeathScentence& ds = AddComponent(DeathScentence, entity);
		ds.deathLoops = 1;
		ds.action = action;

		Damage& damage = AddComponent(Damage, target);
		damage.value = (float)spell_damage;
		damage.sourceEntity = entity;

		if (BehaviourState* bs = GetComponent(BehaviourState, entity))
			damage.hitFrame = bs->GetHitFrame(action);

		return entity;
	}

	const char* GetSpell(int damage, u32 colour)
	{
		std::vector<const char*> candidates;

		for (auto iter = s_spellsRegistry.begin(); iter != s_spellsRegistry.end(); iter++)
		{
			bool within_damage_range = damage > iter->second.damageMin && damage <= iter->second.damageMax;
			if (within_damage_range && (u32)iter->second.colour == colour)
				candidates.push_back(iter->first.c_str());
		}

		if (candidates.size() == 0)
			return nullptr;

		int random_index = Maths::randomNumberBetween(0, (int)candidates.size());
		return candidates[random_index];
	}
}