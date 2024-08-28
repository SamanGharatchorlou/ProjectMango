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
	const Button& getButton(Button::Key key) const;

	Button::State state(Button::Key key, int frame_number) { return getButton(key).state(frame_number); }
	bool isHeld(Button::Key key, int frame_buffer = 0) const;
	bool isPressed(Button::Key key, int frame_buffer = 0);
	bool isReleased(Button::Key key, int frame_buffer = 0);

	int getHeldFrames(Button::Key key) const { return getButton(key).getHeldFrames(); }

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

	bool HandlePressedButton(const Button& button, int frame_buffer);
	bool HandleReleaseButton(const Button& button, int frame_buffer);

public:
	// turn me into an array
	std::vector<Button> mButtons;
	Cursor mCursor;

	// buttons to reset
	std::vector<Button> mPressedButtons;
	std::vector<Button> mReleaseButtons;
};
