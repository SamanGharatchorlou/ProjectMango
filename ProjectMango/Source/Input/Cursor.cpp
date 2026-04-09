#include "pch.h"
#include "Cursor.h"
#include "Graphics/STexture.h"
#include "Game/FrameRateController.h"

Cursor::Cursor() : mTexture(nullptr), mMoving(false), mVisibility(false), mMode(Cursor::None) { }

void Cursor::render(SDL_Renderer* renderer)
{
	if(mTexture)
		mTexture->render(renderer, mRect);
}

void Cursor::setButton(ButtonType type, Button button)
{
	mButtons[type] = button;
}

void Cursor::setCursorOffsetPoint(VectorF offset)
{
	mCursorOffsetPoint = offset;
}


// Query Input
bool Cursor::isHeld(ButtonType type) const
{
	return mButtons[type].isHeld();
}

bool Cursor::isPressed(ButtonType type, int frame_buffer) const
{	
	const FrameRateController& frc = FrameRateController::Get();
	const int frame_count = frc.FrameCount();

	return mButtons[type].isPressed(frame_count - frame_buffer);
}

bool Cursor::isReleased(ButtonType type, int frame_buffer) const
{	
	const FrameRateController& frc = FrameRateController::Get();
	const int frame_count = frc.FrameCount();

	return mButtons[type].isReleased(frame_count - frame_buffer);
}
