#pragma once

#include "UIBox.h"
#include "Input/Button.h"

class Texture;

class UIButton : public UIBox
{
public:
	enum State
	{
		None,
		Hovering,
		Pressed,
		Active
	};

	UIButton();
	UIButton(const StringMap32& attributes);
	~UIButton() { }

	void setTextures(Texture* defaultTexture, Texture* selected, Texture* hovering);

	void reset();

	bool handleInput(InputManager* input) override;

	void setState(State state);
	void setHeld(bool isHeld);
	void setPressed(bool isPressed);
	void setReleased(bool isReleased);

	bool isPressed() const;
	bool isReleased() const;
	inline bool isHeld() const { return mButton.isHeld(); }
	inline int holdCount() const { return mButton.getHeldFrames(); }

	virtual UIType type() const override { return UIType::Button; }

	void setActive(bool active) { mIsActive = active; }
	bool isActive() const { return mIsActive; }

private:
	Texture* mDefault;
	Texture* mSelected;
	Texture* mHovered;

	Button mButton;

	bool mIsActive;
};
