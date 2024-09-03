#include "pch.h"
#include "Cursor.h"
#include "Graphics/Texture.h"
#include "Game/FrameRateController.h"

#if DEBUG_CURSOR
#include "Debugging/DebugDraw.h"
#endif


Cursor::Cursor() : mTexture(nullptr), mMoving(false), mVisibility(false), mMode(Cursor::None) { }

void Cursor::render()
{
	if(mTexture)
		mTexture->render(mRect);
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
