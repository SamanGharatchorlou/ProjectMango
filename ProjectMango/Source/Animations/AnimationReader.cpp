#include "pch.h"
#include "AnimationReader.h"

#include "Animations/Animator.h"
#include "Graphics/TextureManager.h"
#include "Graphics/Texture.h"

static constexpr u32 c_buffLen = 32;
static char s_buffer[c_buffLen];

static void ReadRect(XMLNode root, const char* node, VectorF& pos, VectorF& size)
{
	snprintf(s_buffer, c_buffLen, "%sPosition", node);
	XMLNode pos_node = root.child(s_buffer);
	pos = attributesToVectorF(pos_node);
	
	snprintf(s_buffer, c_buffLen, "%sSize", node);
	XMLNode size_node = root.child(s_buffer);
	size = attributesToVectorF(size_node);
}

static void PopulateBaseSpriteSheet(XMLNode root, SpriteSheet& sheet)
{
	sheet.ID = root.child("ID").value();

	ReadRect(root, "Object", sheet.objectPos, sheet.objectSize);
	ReadRect(root, "Collider", sheet.colliderPos, sheet.colliderSize);

	snprintf(s_buffer, c_buffLen, "ObjectPositionOffset");
	XMLNode pos_node = root.child(s_buffer);
	sheet.objectPosOffseet = attributesToVectorF(pos_node);

	const char* spriteSheet = root.child("SpriteSheet").value();
	Texture* texture = TextureManager::Get()->getTexture(spriteSheet, FileManager::Folder::Image_Animations);
	if (!texture)
	{
		DebugPrint(Error, "No Sprite sheet named %s found for this animation", spriteSheet);
		return;
	}

	sheet.texture = texture;
	sheet.frameSize = attributesToVectorF(root.child("FrameSize"));
	sheet.renderSize = attributesToVectorF(root.child("RenderSize"));
	sheet.boundaries = (texture->originalDimentions / sheet.frameSize).toInt();
}

static void PopulateAnimation(XMLNode node, Animation& animation, const SpriteSheet& sprite_sheet)
{
	animation.action = stringToAction(node.name());

	// convert x,y position in index count, do i need to do this? or better to leave as x,y. cba to change it at this point
	VectorI index = attributesToVectorI(node, "x", "y");
	animation.startIndex = sprite_sheet.boundaries.x * index.y + index.x;
	animation.frameCount = toInt(node.value());

	if (XMLNode::Attribute attributeNode = node.attribute("minLoops"))
	{
		animation.minLoops = toInt(attributeNode->value());
	}

	if (XMLNode::Attribute attributeNode = node.attribute("looping"))
	{
		animation.looping = toBool(attributeNode->value());
	}

	if (XMLNode::Attribute attributeNode = node.attribute("colliderFrame"))
	{
		animation.colliderFrame = toInt(attributeNode->value());
	}

	if (XMLNode::Attribute attributeNode = node.attribute("frameTime"))
	{
		animation.frameTime = toFloat(attributeNode->value());
	}

	// extend the frame time so as to match an animation with targetFrames number of frames in it
	// read after frameTime, we need to respect that new time
	if (XMLNode::Attribute attributeNode = node.attribute("targetFrames"))
	{
		float target_frames = toFloat(attributeNode->value());
		float time_ratio = target_frames / (float)animation.frameCount;
		animation.frameTime *= time_ratio;
	}
}

void AnimationReader::Parse(const char* animation, Animator& animator)
{
	BasicString full_path = FileManager::Get()->findFile(FileManager::Configs, animation);
	XMLParser* parser = new XMLParser;
	parser->parseXML(full_path.c_str());

	const XMLNode root = parser->rootNode();
	XMLNode animatorNode = parser->rootChild("Animator");

	SpriteSheet sheet;
	PopulateBaseSpriteSheet(animatorNode, sheet);
	animator.mSpriteSheets.push_back(sheet);

	float frame_time = toFloat(animatorNode.child("FrameTime").value());;

	XMLNode animations_node = animatorNode.child("Animations");
	while (animations_node)
	{
		SpriteSheet* sprite_sheet = &animator.mSpriteSheets.back();

		int x = 0;
		if(XMLNode::Attribute attribute = animations_node.attribute("x"))
			x = toInt(attribute->value());

		int y = 0;
		if(XMLNode::Attribute attribute = animations_node.attribute("y"))
			y = toInt(attribute->value());

		VectorI direction = VectorI(x,y);

		bool can_flip = false;
		if (XMLNode::Attribute attributeNode = animations_node.attribute("canFlip"))
		{
			can_flip = toBool(attributeNode->value());
		}

		XMLNode single_anim_node = animations_node.child();
		while( single_anim_node )
		{
			Animation animation;

			// popup late common data
			animation.frameTime = frame_time;
			animation.spriteSheetIndex = animator.mSpriteSheets.size() - 1;
			animation.direction = direction;
			animation.canFlip = can_flip;

			// must happen after the common data, we might edit it
			PopulateAnimation(single_anim_node, animation, *sprite_sheet);

			animator.mAnimations.push_back(animation);
			single_anim_node = single_anim_node.next();
		}

		animations_node = animations_node.next("Animations");
	}

	delete parser;
}
