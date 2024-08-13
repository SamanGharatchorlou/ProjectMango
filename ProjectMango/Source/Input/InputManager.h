#pragma once

#include "Button.h"
#include "Cursor.h"

struct InputPacket;

static constexpr int c_inputBuffer = 8;

class InputManager
{
public:
	InputManager() { }
	~InputManager() { }

	static InputManager* Get();

	void init();

	void consumeBufferedInputs();
	void processInputEvent(SDL_Event& event);
	void updateHeldFrame();

	// Buttons
	Button& getButton(Button::Key key);

	Button::State state(Button::Key key, int frame_number) { return getButton(key).state(frame_number); }
	bool isHeld(Button::Key key, int frame_buffer = 0);
	bool isPressed(Button::Key key, int frame_buffer = 0);
	bool isReleased(Button::Key key, int frame_buffer = 0);

	int getHeldFrames(Button::Key key) { return getButton(key).getHeldFrames(); }

	// Cursor
	Cursor* getCursor() { return &mCursor; }
	const Cursor* getCursor() const { return &mCursor; }

	VectorF cursorPosition() const { return mCursor.position(); }

	void setCursorSize(VectorF size);
	bool isCursorHeld(Cursor::ButtonType type) const;
	bool isCursorPressed(Cursor::ButtonType type, int frame_buffer = 0);
	bool isCursorReleased(Cursor::ButtonType type, int frame_buffer = 0);

private:
	void processMouseMovementEvent();
	void processMouseButtonEvent(SDL_Event& event);
	void processButtonEvent(SDL_Event& event);

	void bindDefaultButtons();

	bool HandlePressedButton(Button& button, int frame_buffer);
	bool HandleReleaseButton(Button& button, int frame_buffer);

public:
	// turn me into an array
	std::vector<Button> mButtons;
	Cursor mCursor;

	// buttons to reset
	std::vector<Button> mPressedButtons;
	std::vector<Button> mReleaseButtons;
};
