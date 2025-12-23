#include "pch.h"
#include "AnimationReader.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Collider.h"
#include "Graphics/STexture.h"
#include "Graphics/TextureManager.h"
#include "System/Files/JSONParser.h"
#include "ECS/Components/AIComponents.h"

static std::unordered_map<BasicString, ECS::AnimationSpriteSheet> s_spriteSheets;

namespace AnimationReader
{	
	using namespace rapidjson;

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

	// this runs every time i fire a spell, lets not...
	void BuildAnimatior(ECS::Entity entity, const char* file)
	{
		BasicString full_path = FileManager::Get()->findFile(FileManager::Configs, file);
		if(full_path.length() == 0)
		{
			DebugPrint(PriorityLevel::Log, "Animation file does not exist: '%s'", file);
			return;
		}


		JSONParser parser(full_path.c_str());
		
		if(!parser.document.IsObject())
		{
			DebugPrint(PriorityLevel::Warning, "Invalid animation document: %s", full_path.c_str());
			return;
		} 

		ECS::Animator& animator = GetComponentRef(Animator, entity);
		float frame_size_x = parser.document["frameSize_x"].GetFloat();
		float frame_size_y = parser.document["frameSize_y"].GetFloat();

		//if(ECS::Collider* collider = GetComponent(Collider, entity))
		//{
		//	VectorF object_position = VectorF::zero();
		//	VectorF object_size = VectorF(1, 1);
		//	if(PopulateColliderData("object", parser.document, &object_position, &object_size))
		//	{
		//		collider->SetRelativeRect(object_position, object_size);
		//	}
		//}

		const Value::Array& sprite_sheets = parser.document["spriteSheets"].GetArray();
		for( u32 i = 0; i < sprite_sheets.Size(); i++ )
		{
			const Value& sprite_sheet = sprite_sheets[i];

			const char* spriteSheet_id = sprite_sheet["spriteSheet"].GetString();
			STexture* texture = TextureManager::Get()->getTexture(spriteSheet_id, FileManager::Folder::Image_Animations);
			if (!texture)
			{
				DebugPrint(Error, "No Sprite sheet named %s found for this animation", spriteSheet_id);
			}

			if(!s_spriteSheets.contains(spriteSheet_id))
			{
				ECS::AnimationSpriteSheet spriteSheet;
				spriteSheet.ID = spriteSheet_id;
				spriteSheet.texture = texture;
				spriteSheet.frameSize.x = frame_size_x;
				spriteSheet.frameSize.y = frame_size_y;
				spriteSheet.sheetSize = (texture->originalDimentions / spriteSheet.frameSize).toInt();
				
				s_spriteSheets[spriteSheet_id] = spriteSheet;
			}

			VectorF object_center;
			if(sprite_sheet.HasMember("object_center"))
			{
				const Value& center = sprite_sheet["object_center"];
				object_center = VectorF(center[0].GetFloat(), center[1].GetFloat());
			}

			const Value& anims = sprite_sheet["animations"];
			for( u32 i = 0; i < anims.Size(); i++ )
			{
				ECS::Animation anim;

				const Value& animation = anims[i];
				anim.spriteSheet = s_spriteSheets[spriteSheet_id];
				anim.action = animation.HasMember("action") ? ECS::StringToAction(animation["action"].GetString()) : ECS::Action::None;
				anim.startIndex = animation["startIndex"].GetInt();
				anim.frameCount = animation["frameCount"].GetInt();
				anim.frameTime = animation["frameTime"].GetFloat();
				anim.looping = animation.HasMember("looping") ? animation["looping"].GetBool() : true;
				anim.reversing = animation.HasMember("reverse") ? animation["reverse"].GetBool() : false;

				if( anim.action == ECS::Action::AttackWindUp || anim.action == ECS::Action::BasicAttack )
				{
					ECS::AttackStateData hitbox_data;
					PopulateColliderData("hitbox", animation, &hitbox_data.hitBoxPos, &hitbox_data.hitBoxSize);
					
					if(animation.HasMember("hit_frame"))
						hitbox_data.hitFrame = animation["hit_frame"].GetInt();

					ECS::BehaviourState& beviour_state = GetOrAddComponent(BehaviourState, entity);
					beviour_state.attackData.insert( { anim.action, hitbox_data } );
				}

				animator.animations.push_back(anim);
			}
		}
	}
}
