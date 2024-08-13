#pragma once

class Texture;
enum class ActionState;


struct SpriteSheet
{
	BasicString ID;
	Texture* texture;
	VectorF frameSize;

	VectorI boundaries;

	// outer boundary that we draw
	VectorF renderSize;

	// size of the object/character, the pos is relative to the render rect
	VectorF objectPos;
	VectorF objectSize;

	// todo: move somewhere else?	
	VectorF colliderSize;
	VectorF colliderPos;

	VectorF objectPosOffseet;
};

struct Animation
{
	ActionState action = (ActionState)0;

	VectorI direction;
	bool canFlip = false;

	int spriteSheetIndex = 0;

	int startIndex = 0;
	int frameCount = 0;

	int colliderFrame = 0;

	int minLoops = 0;
	bool looping = true;

	float frameTime = 0.0f;
};