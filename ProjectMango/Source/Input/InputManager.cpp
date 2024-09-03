#include "pch.h"
#include "InputManager.h"

#include "Game/Camera/Camera.h"
#include "Debugging/ImGui/ImGuiMenu.h"
#include "Game/FrameRateController.h"

static void InitButton(Button& button)
{
	button.mPressedFrame = -501;
	button.mReleasedFrame = -500;
	button.mHeldFrames = 0;
}

void InputManager::init()
{
	bindDefaultButtons();

	for (Button& button : mButtons)
	{
		InitButton(button);
	}
	
	InitButton(mCursor.getButton(Cursor::ButtonType::Left));
	InitButton(mCursor.getButton(Cursor::ButtonType::Right));
}

InputManager* InputManager::Get()
{
	GameData& gd = GameData::Get();
	ASSERT(gd.inputManager != nullptr, "Audio manager has no been set up yet");
	return gd.inputManager;
}

void InputManager::consumeBufferedInputs()
{	
	const FrameRateController& frc = FrameRateController::Get();
	const int frame_count = frc.FrameCount();

	for( u32 i = 0; i < mPressedButtons.size(); i++ )
	{
		mPressedButtons[i].mPressedFrame = frame_count - (c_inputBuffer + 1);
	}

	for( u32 i = 0; i < mReleaseButtons.size(); i++ )
	{
		mReleaseButtons[i].mReleasedFrame = frame_count - (c_inputBuffer + 1);
	}
}

void InputManager::processInputEvent(SDL_Event& event)
{
#if IMGUI
	bool has_input = DebugMenu::HandleInput(event);
	if (has_input)
		return;
#endif

	if (event.type == SDL_MOUSEMOTION)
		processMouseMovementEvent();
	else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
		processMouseButtonEvent(event);
	else
		processButtonEvent(event);
}


Button& InputManager::getButton(Button::Key key)
{
	for (unsigned int i = 0; i < mButtons.size(); i++)
	{
		if (mButtons[i].isKey(key))
			return mButtons[i];
	}

	DebugPrint(Warning, "Button for key %d not found, return button with KEY: NONE", key);
	return mButtons[0];
}


const Button& InputManager::getButton(Button::Key key) const
{
	for (unsigned int i = 0; i < mButtons.size(); i++)
	{
		if (mButtons[i].isKey(key))
			return mButtons[i];
	}

	DebugPrint(Warning, "Button for key %d not found, return button with KEY: NONE", key);
	return mButtons[0];
}

bool InputManager::isHeld(Button::Key key, int frame_buffer) const
{ 
	return getButton(key).isHeld(); 
}

bool InputManager::HandlePressedButton(const Button& button, int frame_buffer)
{
	const FrameRateController& frc = FrameRateController::Get();
	const int frame_count = frc.FrameCount();
	const bool is_pressed = button.isPressed(frame_count - frame_buffer);

	// set the pressed frame for this button to be older, so it cant be triggered multiple times
	if(is_pressed)
	{
		//button.mPressedFrame = frame_count - (frame_buffer + 1);
		mPressedButtons.push_back(button);
	}

	return is_pressed;
}

bool InputManager::HandleReleaseButton(const Button& button, int frame_buffer)
{
	const FrameRateController& frc = FrameRateController::Get();
	const int frame_count = frc.FrameCount();
	const bool is_released = button.isReleased(frame_count - frame_buffer);

	// set the released frame for this button to be older, so it cant be triggered multiple times
	if(is_released)
	{
		//button.mReleasedFrame = frame_count - (frame_buffer + 1);
		mReleaseButtons.push_back(button);
	}

	return is_released;
}

bool InputManager::isPressed(Button::Key key, int frame_buffer)
{ 
	const Button& button = getButton(key);
	return HandlePressedButton(button, frame_buffer);
}

bool InputManager::isReleased(Button::Key key, int frame_buffer)
{ 	
	const Button& button = getButton(key);
	return HandleReleaseButton(button, frame_buffer);
}

// Cursor
void InputManager::setCursorSize(VectorF size) { mCursor.setSize(size); }
bool InputManager::isCursorHeld(Cursor::ButtonType button) const { return mCursor.isHeld(button); }
bool InputManager::isCursorPressed(Cursor::ButtonType cursor_button, int frame_buffer)
{ 
	const Button& button = mCursor.getButton(cursor_button);
	return HandlePressedButton(button, frame_buffer);
}
bool InputManager::isCursorReleased(Cursor::ButtonType cursor_button, int frame_buffer)
{ 
	Button& button = mCursor.getButton(cursor_button);
	return HandleReleaseButton(button, frame_buffer);
}

// --- Private Functions --- //
void InputManager::processMouseMovementEvent()
{
	int x, y;
	SDL_GetMouseState(&x, &y);

	float scale = Camera::Get()->scale();
	VectorF cursorPostion = VectorF((float)x, (float)y) / scale;

	mCursor.setPosition(cursorPostion);
	mCursor.setMotion(true);
}

void InputManager::processMouseButtonEvent(SDL_Event& event)
{
	const FrameRateController& frc = FrameRateController::Get();
	const int frame_count = frc.FrameCount();

	SDL_MouseButtonEvent buttonEvent = event.button;

	Cursor::ButtonType buttonType = Cursor::ButtonType::Count;
	if (buttonEvent.button == Button::LeftClick)
		buttonType = Cursor::Left;
	else if (buttonEvent.button == Button::rightClick)
		buttonType = Cursor::Right;
	else
		DebugPrint(Warning, "Mouse button type %d not left or right", buttonEvent.button);

	// Get input data
	Button& cursorButton = mCursor.getButton(buttonType);

	if(event.type == SDL_MOUSEBUTTONUP)
	{
		cursorButton.setReleased(frame_count);
	}

	if( event.type == SDL_MOUSEBUTTONDOWN )
	{
		if(cursorButton.mReleasedFrame > cursorButton.mPressedFrame)
		{
			cursorButton.setPressed(frame_count);
		}
	}
}

void InputManager::processButtonEvent(SDL_Event& event)
{ 
	const FrameRateController& frc = FrameRateController::Get();
	const int frame_count = frc.FrameCount();

	for (Button& button : mButtons)
	{
		if (button.isKey(event.key.keysym.sym))
		{
			if(event.type == SDL_KEYUP)
			{
				button.setReleased(frame_count);
			}

			if(event.type == SDL_KEYDOWN)
			{
				if(button.mReleasedFrame > button.mPressedFrame)
				{
					button.setPressed(frame_count);
				}
			}
		}
	}
}

static void HandleButtonHoldFrameCounter(Button& button)
{
	const bool is_held = button.mPressedFrame > button.mReleasedFrame;
	if(!is_held)
		button.setHeldFrames(0);
	else
		button.incrementHeldFrames();
}

void InputManager::updateHeldFrame()
{
	for (Button& button : mButtons)
	{
		HandleButtonHoldFrameCounter(button);
	}

	HandleButtonHoldFrameCounter(mCursor.getButton(Cursor::ButtonType::Left));
	HandleButtonHoldFrameCounter(mCursor.getButton(Cursor::ButtonType::Right));
}

void InputManager::bindDefaultButtons()
{
	mButtons.push_back(Button(Button::Key::None));

	// Movement
	mButtons.push_back(Button(Button::Key::Up));
	mButtons.push_back(Button(Button::Key::Down));
	mButtons.push_back(Button(Button::Key::Left));
	mButtons.push_back(Button(Button::Key::Right));

	mButtons.push_back(Button(Button::Key::LeftArrow));
	mButtons.push_back(Button(Button::Key::RightArrow));
	mButtons.push_back(Button(Button::Key::DownArrow));
	mButtons.push_back(Button(Button::Key::UpArrow));

	// Game state
	mButtons.push_back(Button(Button::Key::Pause));
	mButtons.push_back(Button(Button::Key::Esc));
	mButtons.push_back(Button(Button::Key::Quit));
	mButtons.push_back(Button(Button::Key::Enter));
	mButtons.push_back(Button(Button::Key::Ctrl));
	mButtons.push_back(Button(Button::Key::Space));
	mButtons.push_back(Button(Button::Key::Shift));

	// Keys
	mButtons.push_back(Button(Button::Key::E));

	// Numbers
	mButtons.push_back(Button(Button::Key::Zero));
	mButtons.push_back(Button(Button::Key::One));
	mButtons.push_back(Button(Button::Key::Two));
	mButtons.push_back(Button(Button::Key::Three));
	mButtons.push_back(Button(Button::Key::Four));
	mButtons.push_back(Button(Button::Key::Five));
	mButtons.push_back(Button(Button::Key::Six));
	mButtons.push_back(Button(Button::Key::Seven));
	mButtons.push_back(Button(Button::Key::Eight));
	mButtons.push_back(Button(Button::Key::Nine));

	// F numbers
	mButtons.push_back(Button(Button::Key::F1));
	mButtons.push_back(Button(Button::Key::F2));
	mButtons.push_back(Button(Button::Key::F3));
	mButtons.push_back(Button(Button::Key::F4));
	mButtons.push_back(Button(Button::Key::F5));
	mButtons.push_back(Button(Button::Key::F6));
	mButtons.push_back(Button(Button::Key::F7));
	mButtons.push_back(Button(Button::Key::F8));
	mButtons.push_back(Button(Button::Key::F9));
	mButtons.push_back(Button(Button::Key::F10));
	mButtons.push_back(Button(Button::Key::F11));
	mButtons.push_back(Button(Button::Key::F12));
}
