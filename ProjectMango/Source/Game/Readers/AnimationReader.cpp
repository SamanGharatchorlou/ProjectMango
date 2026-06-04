#include "pch.h"
#include "AnimationReader.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/GraphicComponents.h"
#include "Graphics/STexture.h"
#include "Graphics/TextureManager.h"
#include "System/Files/JSONParser.h"
#include "ECS/Components/AIComponents.h"

using namespace ECS;

struct AnimatorData
{
	 std::vector<Animation> animations;
	 std::unordered_map<Action::Enum, AttackStateData> attackStateData;
};

static std::unordered_map< BasicString, AnimatorData > s_animationData;

namespace AnimationReader
{	
	using namespace rapidjson;

	void ClearAnimationData()
	{
		s_animationData.clear();
	}

	static bool PopulateColliderData(const char* prefix, const Value& animation, VectorF* out_pos, VectorF* out_size)
	{
		bool has_data = false;

		const u32 len = 64;
		char pos[len];
		snprintf(pos, len, "%s_pos", prefix);
		char size[len];
		snprintf(size, len, "%s_size", prefix);

		if (out_pos && animation.HasMember(pos))
		{
			const Value& relative_pos = animation[pos];

			out_pos->x = relative_pos[0].GetFloat();
			out_pos->y = relative_pos[1].GetFloat();
			has_data = true;
		}
		if (out_size && animation.HasMember(size))
		{
			const Value& relative_size = animation[size];

			out_size->x = relative_size[0].GetFloat();
			out_size->y = relative_size[1].GetFloat();
			has_data = true;
		}

		return has_data;
	}
	
	bool AnimationExists(const char* animiation_id)
	{
		return s_animationData.contains(animiation_id);
	}


	VectorF GetAnimationFrameSize(const char* animiation_id)
	{
		if (AnimationExists(animiation_id))
		{
			if (s_animationData.at(animiation_id).animations.size() > 0)
			{
				return s_animationData.at(animiation_id).animations[0].frame.frameSize;
			}
		}

		return VectorF::zero();
	}

	static void AddAttackData(Entity entity, const AnimatorData& animator_data, Action::Enum action)
	{
		if(animator_data.attackStateData.contains(action))
		{
			BehaviourState& beviour_state = GetOrAddComponent(BehaviourState, entity);
			beviour_state.attackData.insert( { action, animator_data.attackStateData.at(action) } );
		}
	}
	
	void BuildAnimator(Entity entity, const char* animator_id)
	{
		if(!animator_id || !s_animationData.contains(animator_id))
			return;

		const AnimatorData& animator_data = s_animationData.at(animator_id);

		Animator& animator = GetComponentRef(Animator, entity);
		animator.animations = &animator_data.animations;
		
		for( u32 i = 0; i < animator.animations->size(); i++ )
		{
			AddAttackData(entity, animator_data, animator.animations->at(i).action);
		}
	}

	
	void Debug_GetAnimationIDs(std::vector<BasicString>& out_ids)
	{
		std::vector<BasicString> files;
		FileManager::Get()->GetFilesInFolder(FileManager::Config_Animations, files);

		for( const BasicString& file : files )
		{
			JSONParser parser(file.c_str());
			if(!parser.IsValid())
				continue;
			
			if(parser.document.HasMember("vfx"))
			{
				Value& types = parser.document["vfx"];
				if (types.IsArray())
				{
					for (u32 i = 0; i < types.Size(); i++)
					{
						const char* id = types[i]["id"].GetString();
						out_ids.emplace_back(BasicString(id));
					}
				}
			}
			else
			{
				const char* id = parser.document["id"].GetString();
				out_ids.emplace_back(BasicString(id));
			}
		}
	}

	static bool PopulateFrameData(const Value& data, const Animation& defaults, Animation& out_animation)
	{
		const char* sprite_sheet_id = defaults.image.id.c_str();
		if (data.HasMember("spriteSheet"))
			sprite_sheet_id = data["spriteSheet"].GetString();

		STexture* texture = TextureManager::Get()->getTexture(sprite_sheet_id, FileManager::Folder::Image_Animations);
		if (!texture)
		{
			DebugPrint(Error, "No Sprite sheet named %s found for this animation", sprite_sheet_id);
			return false;
		}

		out_animation.action = data.HasMember("action") ? StringToAction(data["action"].GetString()) : defaults.action;

		// sprite sheet image
		out_animation.image.texture = texture;
		out_animation.image.id = sprite_sheet_id;

		// frames
		out_animation.frame.frameSize.x = data.HasMember("frameSize") ? data["frameSize"][0].GetFloat() : defaults.frame.frameSize.x;
		out_animation.frame.frameSize.y = data.HasMember("frameSize") ? data["frameSize"][1].GetFloat() : defaults.frame.frameSize.y;
		out_animation.frame.gridCount = (texture->originalDimentions / out_animation.frame.frameSize).toInt();
		out_animation.frameTime = data["frameTime"].GetFloat();
		out_animation.startIndex = data["startIndex"].GetInt();
		out_animation.frameCount = data["frameCount"].GetInt();

		// format
		out_animation.looping = data.HasMember("looping") ? data["looping"].GetBool() : defaults.looping;
		out_animation.reversing = data.HasMember("reverse") ? data["reverse"].GetBool() : defaults.reversing;

		return true;
	}

	static bool PopulateAttackStateData(const Value& data, AttackStateData& asd)
	{		
		// attack data
		bool has_attack_data = PopulateColliderData("hitbox", data, &asd.hitBoxPos, &asd.hitBoxSize);

		if (data.HasMember("hit_frame"))
		{
			asd.hitFrame = data["hit_frame"].GetInt();
			has_attack_data = true;
		}

		if (data.HasMember("hit_vfx"))
		{
			asd.hitVfx = data["hit_vfx"].GetString();
			has_attack_data = true;
		}

		if (data.HasMember("attack_vfx"))
		{
			asd.attackVfx = data["attack_vfx"].GetString();
			has_attack_data = true;
		}

		if (data.HasMember("damage"))
		{
			asd.damage = data["damage"].GetFloat();
			has_attack_data = true;
		}

		if (data.HasMember("debuff"))
		{
			asd.debuff = data["debuff"].GetString();
			has_attack_data = true;
		}

		return has_attack_data;
	}

	// gets each individual animation and puts it into the map
	// problem is i need them packaged up into lists so i can make an animator out of it
	// so just turn them into lists instead?
	void ReadAnimationData()
	{
		std::vector<BasicString> files;
		FileManager::Get()->GetFilesInFolder(FileManager::Config_Animations, files);

		for( const BasicString& file : files )
		{
			JSONParser parser(file.c_str());
			if(!parser.IsValid())
				continue;
			
			if(parser.document.HasMember("vfx"))
			{
				const Value& types = parser.document["vfx"];
				if (!types.IsArray())
					continue;

				Animation defaults;
				defaults.action = Action::Active;
				defaults.looping = false;

				for (u32 i = 0; i < types.Size(); i++)
				{
					const Value& vfx_data = types[i];

					const char* id = vfx_data["id"].GetString();
					std::vector<Animation>& animations = s_animationData[id].animations;
					animations.push_back( Animation() );
					Animation& animation = animations.back();

					PopulateFrameData(vfx_data, defaults, animation);

					AttackStateData asd;
					if (PopulateAttackStateData(vfx_data, asd))
					{
						s_animationData[id].attackStateData.insert({ animation.action, asd });
					}

					if (vfx_data.HasMember("inactiveFrame"))
					{
						animations.push_back(animation);

						Animation& inactive_animation = animations.back();
						inactive_animation.action = Action::Inactive;
						inactive_animation.startIndex = vfx_data["inactiveFrame"].GetInt();
						inactive_animation.frameCount = 1;
					}
				}
			}
			else
			{
				const char* id = parser.document["id"].GetString();
				std::vector<Animation>& animations = s_animationData[id].animations;

				Animation defaults;
				defaults.frame.frameSize.x = parser.document["frameSize_x"].GetFloat();
				defaults.frame.frameSize.y = parser.document["frameSize_y"].GetFloat();

				const Value::Array& sprite_sheets = parser.document["spriteSheets"].GetArray();
				for( u32 j = 0; j < sprite_sheets.Size(); j++ )
				{
					const Value& sprite_sheet = sprite_sheets[j];

					defaults.image.id = sprite_sheet["spriteSheet"].GetString();

					const Value& anims = sprite_sheet["animations"];
					for( u32 i = 0; i < anims.Size(); i++ )
					{
						animations.push_back( Animation() );
						Animation& animation = animations.back();

						PopulateFrameData(anims[i], defaults, animation);

						AttackStateData asd;
						if (PopulateAttackStateData(anims[i], asd))
						{
							s_animationData[id].attackStateData.insert({ animation.action, asd });
						}
					}
				}
			}
		}
	}
}
