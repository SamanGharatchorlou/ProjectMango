#include "pch.h"
#include "AnimationReader.h"

#include "Animations/CharacterStates.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Animator.h"
#include "ECS/EntityCoordinator.h"
#include "Graphics/Texture.h"
#include "Graphics/TextureManager.h"
#include "System/Files/JSONParser.h"

static std::unordered_map<BasicString, ECS::SpriteSheet> s_spriteSheets;

namespace AnimationReader
{	
	using namespace rapidjson;

	static void PopulateColliderData(const char* prefix, const Value& animation, VectorF* out_pos, VectorF* out_size)
	{
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
		}
		if (out_size && animation.HasMember(size))
		{
			const Value& relative_size = animation[size];

			out_size->x = relative_size[0].GetFloat();
			out_size->y = relative_size[1].GetFloat();
		}
	}

	void BuildAnimatior(ECS::Entity entity, const char* file)
	{
		BasicString full_path = FileManager::Get()->findFile(FileManager::Configs, file);
		JSONParser parser(full_path.c_str());
		
		if(!parser.document.IsObject())
		{
			DebugPrint(PriorityLevel::Warning, "Invalid animation document: %s", full_path.c_str());
			return;
		} 

		ECS::EntityCoordinator* ecs = GameData::Get().ecs;

		float frame_size_x = parser.document["frameSize_x"].GetFloat();
		float frame_size_y = parser.document["frameSize_y"].GetFloat();

		const Value::Array& sprite_sheets = parser.document["spriteSheets"].GetArray();
		for( u32 i = 0; i < sprite_sheets.Size(); i++ )
		{
			const Value& sprite_sheet = sprite_sheets[i];

			const char* spriteSheet_id = sprite_sheet["spriteSheet"].GetString();
			Texture* texture = TextureManager::Get()->getTexture(spriteSheet_id, FileManager::Folder::Image_Animations);
			if (!texture)
			{
				DebugPrint(Error, "No Sprite sheet named %s found for this animation", spriteSheet_id);
			}

			if(!s_spriteSheets.contains(spriteSheet_id))
			{
				ECS::SpriteSheet spriteSheet;
				spriteSheet.ID = spriteSheet_id;
				spriteSheet.texture = texture;
				spriteSheet.frameSize.x = frame_size_x;
				spriteSheet.frameSize.y = frame_size_y;
				spriteSheet.sheetSize = (texture->originalDimentions / spriteSheet.frameSize).toInt();

				s_spriteSheets[spriteSheet_id] = spriteSheet;
			}

			const Value& anims = sprite_sheet["animations"];
			for( u32 i = 0; i < anims.Size(); i++ )
			{
				ECS::Animation anim;

				const Value& animation = anims[i];
				anim.spriteSheet = s_spriteSheets[spriteSheet_id];
				anim.action = StringToAction(animation["action"].GetString());
				anim.startIndex = animation["startIndex"].GetInt();
				anim.frameCount = animation["frameCount"].GetInt();
				anim.frameTime = animation["frameTime"].GetFloat();
				anim.looping = animation.HasMember("looping") ? animation["looping"].GetBool() : true;
				anim.reversing = animation.HasMember("reverse") ? animation["reverse"].GetBool() : false;
				anim.attackColliderFrameStart = animation.HasMember("attack_collider_frame_start") ? animation["attack_collider_frame_start"].GetInt() : -1;
				anim.attackColliderFrameEnd = animation.HasMember("attack_collider_frame_end") ? animation["attack_collider_frame_end"].GetInt() : -1;

				anim.flipPointX = 0.5f;
				if (animation.HasMember("flip_point_x"))
				{
					anim.flipPointX = animation["flip_point_x"].GetFloat();
				}

				anim.entityColliderPos = VectorF::zero();
				anim.entityColliderSize = VectorF(1, 1);
				PopulateColliderData("entity_collider", animation, &anim.entityColliderPos, &anim.entityColliderSize);
				PopulateColliderData("attack_collider", animation, &anim.attackColliderPos, &anim.attackColliderSize);
				PopulateColliderData("entity_collider_end", animation, &anim.entityColliderEndPos, nullptr);
				
				ECS::Animator& animator = ecs->GetComponentRef(Animator, entity);
				animator.animations.push_back(anim);
			}
		}
	}
}