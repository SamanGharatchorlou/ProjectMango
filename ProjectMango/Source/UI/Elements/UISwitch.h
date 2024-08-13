#pragma once

#include "UIBox.h"
#include "Input/Button.h"


class Texture;

class UISwitch : public UIBox
{
public:
	enum State
	{
		Off,
		On
	};

	UISwitch(const StringMap32& attributes);

	void setTextures(Texture* off, Texture* on);

	void reset() { }
	bool handleInput(InputManager* input) override;

	void setState(State state);
	void switchState();
	State state() const { return mState; }

	virtual UIType type() const { return UIType::Switch; }


private:
	Texture* mOn;
	Texture* mOff;

	State mState;
};