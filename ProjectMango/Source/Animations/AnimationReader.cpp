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

	void BuildAnimatior(const char* file, ECS::Animator& animator)
	{
		BasicString full_path = FileManager::Get()->findFile(FileManager::Configs, file);
		JSONParser parser(full_path.c_str());
		
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

		Value& animations = parser.document["animations"];
		for( u32 i = 0; i < animations.Size(); i++ )
		{
			Value& animation = animations[i];

			const char* action = animation["action"].GetString();
			int start_index = animation["startIndex"].GetInt();
			int frame_count = animation["frameCount"].GetInt();
			float frame_time = animation["frameTime"].GetFloat();
			float looping = animation.HasMember("looping") ? animation["looping"].GetBool() : true;

			animator.animations.push_back(ECS::Animation());
			ECS::Animation& anim = animator.animations.back();

			anim.action = stringToAction(action);
			anim.frameCount = frame_count;
			anim.frameTime = frame_time;
			anim.startIndex = start_index;
			anim.spriteSheet = &s_spriteSheets[id];
			anim.looping = looping;
		}
	}
}