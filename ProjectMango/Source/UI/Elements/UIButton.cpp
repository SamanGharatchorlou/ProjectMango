#include "pch.h"
#include "UIButton.h"

#include "Input/InputManager.h"
#include "Game/FrameRateController.h"

UIButton::UIButton(const StringMap32& attributes) :
	UIBox(attributes), mIsActive(false),
	mDefault(nullptr), mSelected(nullptr), mHovered(nullptr) { }

UIButton::UIButton() : mIsActive(false), mDefault(nullptr), mSelected(nullptr), mHovered(nullptr) { }

void UIButton::setTextures(Texture* defaultTexture, Texture* selected, Texture* hovering)
{
	mDefault = defaultTexture;

	mSelected = selected != nullptr ? selected : defaultTexture;
	mHovered = hovering != nullptr ? hovering : defaultTexture;

	mTexture = mDefault;
}


bool UIButton::handleInput(InputManager* input)
{
	bool hasPressed = false;
	if (isPointInBounds(input->cursorPosition()))
	{
		setState(UIButton::State::Hovering);
		setHeld(input->isCursorHeld(Cursor::Left));
		setReleased(input->isCursorReleased(Cursor::Left));

		if (input->isCursorPressed(Cursor::Left))
		{
			setPressed(true);
			hasPressed = true;
		}
	}
	else
	{
		reset();
	}

	return hasPressed;
}


void UIButton::reset()
{
	mButton.setPressed(false);
	mButton.setReleased(false);
	//mButton.setHeld(false);
	mButton.setHeldFrames(0);

	setActive(false);
	setState(State::None);
}


void UIButton::setState(State state)
{
	switch (state)
	{
	case UIButton::None:
		mTexture = mDefault;
		break;
	case UIButton::Hovering:
		mTexture = mHovered;
		break;
	case UIButton::Pressed:
	case UIButton::Active:
		mTexture = mSelected;
		break;
	default:
		DebugPrint(Warning, "Not a valid button state %d", state);
		break;
	}
}

void UIButton::setPressed(bool isPressed) 
{ 
	mButton.setPressed(isPressed);

	if (isPressed)
		setState(State::Pressed);
}


void UIButton::setHeld(bool isHeld)
{
	//mButton.setHeld(isHeld);
	mButton.mHeldFrames = 1;

	if (isHeld)
		setState(State::Pressed);
}


void UIButton::setReleased(bool isReleased) 
{ 
	mButton.setReleased(isReleased);

	if (isReleased)
		setState(State::Active);
}

bool UIButton::isPressed() const 
{ 
	const FrameRateController& frc = FrameRateController::Get();
	const int frame_count = frc.frameCount();

	return mButton.isPressed(frame_count); 
}

bool UIButton::isReleased() const 
{ 	
	const FrameRateController& frc = FrameRateController::Get();
	const int frame_count = frc.frameCount();

	return mButton.isReleased(frame_count); 
}