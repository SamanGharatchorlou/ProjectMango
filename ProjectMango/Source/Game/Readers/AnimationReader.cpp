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

	static void AddAttackData(Entity entity, const AnimatorData& animator_data, Action::Enum action)
	{
		if(animator_data.attackStateData.contains(action))
		{
			BehaviourState& beviour_state = GetOrAddComponent(BehaviourState, entity);
			beviour_state.attackData.insert( { action, animator_data.attackStateData.at(action) } );
		}
	}

	// this runs every time i fire a spell, lets not...
	void BuildAnimator(Entity entity, const char* animator_id)
	{
		if(animator_id && !s_animationData.contains(animator_id))
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
				Value& types = parser.document["vfx"];
				if (types.IsArray())
				{
					for (u32 i = 0; i < types.Size(); i++)
					{
						Value& vfx_data = types[i];
						const char* id = vfx_data["id"].GetString();

						const char* spriteSheet_id = vfx_data["spriteSheet"].GetString();
						STexture* texture = TextureManager::Get()->getTexture(spriteSheet_id, FileManager::Folder::Image_Animations);
						if (!texture)
						{
							DebugPrint(Error, "No Sprite sheet named %s found for this animation", spriteSheet_id);
							continue;
						}
					
						std::vector<Animation>& animations = s_animationData[id].animations;
						animations.push_back( Animation() );
						Animation& animation = animations.back();

						// vfx defaults
						animation.looping = false;
						animation.action =  Action::Active;
					
						// sprite sheet image
						animation.image.texture = texture;
						animation.image.id = spriteSheet_id;

						// frames
						animation.frame.frameSize.x = vfx_data["frameSize"][0].GetFloat();
						animation.frame.frameSize.y = vfx_data["frameSize"][1].GetFloat();
						animation.frame.gridCount = (texture->originalDimentions / animation.frame.frameSize).toInt();
						animation.frameTime = vfx_data["frameTime"].GetFloat();
						animation.startIndex = vfx_data["startIndex"].GetInt();;
						animation.frameCount = vfx_data["frameCount"].GetInt();;
					}
				}
			}
			else
			{
				const char* id = parser.document["id"].GetString();
				std::vector<Animation>& animations = s_animationData[id].animations;

				float frame_size_x = parser.document["frameSize_x"].GetFloat();
				float frame_size_y = parser.document["frameSize_y"].GetFloat();

				const Value::Array& sprite_sheets = parser.document["spriteSheets"].GetArray();
				for( u32 i = 0; i < sprite_sheets.Size(); i++ )
				{
					const Value& sprite_sheet = sprite_sheets[i];

					const char* spriteSheet_id = sprite_sheet["spriteSheet"].GetString();
					STexture* texture = TextureManager::Get()->getTexture(spriteSheet_id, FileManager::Folder::Image_Animations);
					if (!texture)
					{
						DebugPrint(Error, "No Sprite sheet named %s found for this animation", spriteSheet_id);
						continue;
					}

					const Value& anims = sprite_sheet["animations"];
					for( u32 i = 0; i < anims.Size(); i++ )
					{
						animations.push_back( Animation() );
						Animation& animation = animations.back();

						// image data
						animation.image.id = spriteSheet_id;
						animation.image.texture = texture;

						// frame data
						animation.frame.frameSize.x = frame_size_x;
						animation.frame.frameSize.y = frame_size_y;
						animation.frame.gridCount = (texture->originalDimentions / animation.frame.frameSize).toInt();
				
						// animation data
						animation.action = anims[i].HasMember("action") ? StringToAction(anims[i]["action"].GetString()) : Action::None;
						animation.startIndex = anims[i]["startIndex"].GetInt();
						animation.frameCount = anims[i]["frameCount"].GetInt();
						animation.frameTime = anims[i]["frameTime"].GetFloat();
						animation.looping = anims[i].HasMember("looping") ? anims[i]["looping"].GetBool() : true;
						animation.reversing = anims[i].HasMember("reverse") ? anims[i]["reverse"].GetBool() : false;

						// attack data
						bool has_attack_data = anims[i].HasMember("hitbox_size");
						if( has_attack_data )
						{
							AttackStateData hitbox_data;
							PopulateColliderData("hitbox", anims[i], &hitbox_data.hitBoxPos, &hitbox_data.hitBoxSize);
					
							if(anims[i].HasMember("hit_frame"))
								hitbox_data.hitFrame = anims[i]["hit_frame"].GetInt();

							if(anims[i].HasMember("hit_vfx"))
								hitbox_data.hitVfx = anims[i]["hit_vfx"].GetString();
							
							if(anims[i].HasMember("attack_vfx"))
								hitbox_data.attackVfx = anims[i]["attack_vfx"].GetString();
							
							//hitbox_data.attackFrame = hitbox_data.hitFrame;
							//if(anims[i].HasMember("attack_frame"))
							//	hitbox_data.attackFrame = anims[i]["attack_frame"].GetInt();
							
							if(anims[i].HasMember("damage_ratio"))
								hitbox_data.damageRatio = anims[i]["damage_ratio"].GetFloat();

							s_animationData[id].attackStateData.insert( {animation.action, hitbox_data } );
						}
					}
				}
			}
		}
	}
}
