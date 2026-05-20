#include "pch.h"
#include "SpellRegistry.h"

#include "System/Files/JSONParser.h"

#include "Entities/Factory/EntityBuilder.h"
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
		int damage = 0;
		Colour::Type colour = Colour::Count;

		bool operator == (CardSpell& cs) { return damage == cs.damage && colour == cs.colour; }
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
		FileManager::Get()->FindFile(FileManager::Configs, file, file_path);
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
						spell.damage = value["damage"].GetInt();
						spell.size = value["size"].GetFloat();
						spell.colour = Colour::s_stringToType.at(value["colour"].GetString());

						s_spellsRegistry[value["id"].GetString()] = spell;
					}
				}
			}
		}

		// default populate any non-existing entries
		for (u32 i = 0; i < Colour::Count; i++)
		{
			for (int j = 1; j <= 5; j++)
			{
				CardSpell spell;
				spell.damage = j;
				spell.colour = (Colour::Type)i;

				if (SpellExists(spell))
					continue;

				char id_buffer[32];
				snprintf(id_buffer, 32, "%s_spell_%d", Colour::s_typeToString.at(spell.colour).c_str(), spell.damage);

				s_spellsRegistry[id_buffer] = spell;
			}
		}
	}

	void GetSpellMetaData(const char* spell_id, ECS::EntityMetaData& emd)
	{
		if (!spell_id || !AnimationReader::AnimationExists(spell_id))
			return;

		const CardSpell& spell = s_spellsRegistry.at(spell_id);

		VectorF size = AnimationReader::GetAnimationFrameSize(spell_id);
		size = size * spell.size;

		emd.data.strings["Id"] = spell_id;
		emd.data.strings["Sprite"] = spell_id;
		emd.data.vectors["Size"] = size;
		emd.data.values["Damage"] = (float)spell.damage;
		emd.data.values["colour"] = (float)spell.colour;
	}

	Entity CreateSpell(const char* spell_id, ECS::Entity target)
	{
		if (!spell_id || !AnimationReader::AnimationExists(spell_id))
			return EntityInvalid;

		const CardSpell& spell = s_spellsRegistry.at(spell_id);

		VectorF size = AnimationReader::GetAnimationFrameSize(spell_id);
		size = size * spell.size;

		RectF target_rect = GetRect(target);
		float tl_x = GetPosition(target).x - size.x * 0.5f;
		float tl_y = target_rect.BotPoint() - size.y;

		EntityMetaData data;
		data.data.strings["Id"] = spell_id;
		data.data.vectors["Position"] = VectorF(tl_x, tl_y);
		data.data.vectors["Size"] = size;
		data.data.strings["Sprite"] = spell_id;

		Entity entity = CreateBasicObject(data);

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
		damage.value = (float)spell.damage;
		damage.sourceEntity = entity;

		if (BehaviourState* bs = GetComponent(BehaviourState, entity))
			damage.hitFrame = bs->GetHitFrame(action);

		return entity;
	}

	const char* GetSpell(int points, u32 colour)
	{
		std::vector<const char*> candidates;

		for (auto iter = s_spellsRegistry.begin(); iter != s_spellsRegistry.end(); iter++)
		{
			if (iter->second.damage == points && (u32)iter->second.colour == colour)
				candidates.push_back(iter->first.c_str());
		}

		if (candidates.size() == 0)
			return nullptr;

		int random_index = Maths::randomNumberBetween(0, (int)candidates.size());
		return candidates[random_index];
	}
}