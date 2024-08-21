#include "pch.h"
#include "AnimationReader.h"

#include "Animations/CharacterStates.h"
#include "Core/Helpers.h"
#include "ECS/Components/Animator.h"
#include "Graphics/Texture.h"
#include "Graphics/TextureManager.h"
#include "System/Files/JSONParser.h"

static std::unordered_map<const char*, ECS::SpriteSheet> s_spriteSheets;

namespace AnimationReader
{	
	using namespace rapidjson;

	void BuildAnimatior(const char* file, std::vector<ECS::Animation>& animations)
	{
		BasicString full_path = FileManager::Get()->findFile(FileManager::Configs, file);
		JSONParser parser(full_path.c_str());
		
		if(!parser.document.IsObject())
			return;

		const char* id = parser.document["id"].GetString();

		const char* spriteSheet_id = parser.document["spriteSheet"].GetString();
		Texture* texture = TextureManager::Get()->getTexture(spriteSheet_id, FileManager::Folder::Image_Animations);
		if (!texture)
		{
			DebugPrint(Error, "No Sprite sheet named %s found for this animation", spriteSheet_id);
		}

		if(!s_spriteSheets.contains(id))
		{
			ECS::SpriteSheet spriteSheet;
			spriteSheet.ID = id;
			spriteSheet.texture = texture;
			spriteSheet.frameSize.x = parser.document["frameSize_x"].GetFloat();
			spriteSheet.frameSize.y = parser.document["frameSize_y"].GetFloat();
			spriteSheet.sheetSize = (texture->originalDimentions / spriteSheet.frameSize).toInt();

			s_spriteSheets[id] = spriteSheet;
		}

		Value& anims = parser.document["animations"];
		for( u32 i = 0; i < anims.Size(); i++ )
		{
			animations.push_back(ECS::Animation());
			ECS::Animation& anim =animations.back();

			Value& animation = anims[i];
			anim.spriteSheet = &s_spriteSheets[id];
			anim.action = stringToAction(animation["action"].GetString());
			anim.startIndex = animation["startIndex"].GetInt();
			anim.frameCount = animation["frameCount"].GetInt();
			anim.frameTime = animation["frameTime"].GetFloat();
			anim.looping = animation.HasMember("looping") ? animation["looping"].GetBool() : true;

			anim.colliderPos = VectorF::zero();
			if(animation.HasMember("collider_pos"))
			{
				Value& relative_pos = animation["collider_pos"];

				anim.colliderPos.x = relative_pos[0].GetFloat();
				anim.colliderPos.y = relative_pos[1].GetFloat();
			}

			anim.colliderSize = VectorF(1,1);
			if(animation.HasMember("collider_size"))
			{
				Value& relative_size = animation["collider_size"];

				anim.colliderSize.x = relative_size[0].GetFloat();
				anim.colliderSize.y = relative_size[1].GetFloat();
			}

			anim.flipPointX = 0.5f;
			if(animation.HasMember("flip_point_x"))
			{
				anim.flipPointX = animation["flip_point_x"].GetFloat();
			}
		}
	}
}