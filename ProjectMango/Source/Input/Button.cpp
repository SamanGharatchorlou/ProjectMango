#include "pch.h"
#include "Button.h"

Button::Button() : mKeyBinding(Key::None), mHeldFrames(0), mPressedFrame(0), mReleasedFrame(0) { }

Button::Button(Key key) : mKeyBinding(key), mHeldFrames(0), mPressedFrame(0), mReleasedFrame(0) { }

Button::State Button::state(int frame_number) const
{
	if (mPressedFrame == frame_number)
		return State::Pressed;
	else if (mReleasedFrame == frame_number)
		return State::Released;
	else if (mHeldFrames > 0)
		return State::Held;
	else
		return State::None;
}