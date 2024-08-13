#pragma once

#include "Game/States/State.h"
#include "UI/ScreenTypes.h"
#include "UI/ScreenLayers.h"

class UIButton;
class UIElement;
class UISlider;
class UITextBox;
class UISwitch;
class InputManager;


class Screen : public State
{
public:
	enum Flags
	{
		RenderBelow = 1 << 0,
		UpdateBelow = 1 << 1,
		HanleBelowInputs = 1 << 2
	};

	Screen() : mFlags(0) { }
	~Screen();

	void add(const ScreenLayer& layer);
	void add(const ScreenLayers& layers);

	ScreenLayers& layers() { return mScreenLayers; }
	std::unordered_map<StringBuffer32, UIElement*>& iteractables() { return mInteractables; }

	virtual void updateInputs(InputManager* input);
	virtual void handleInput(InputManager* input) { };
	virtual void Update() { };
	void render();

	virtual UIScreen::Type type() { return UIScreen::Type::None; }

	UIElement* find(const char* id);
	UIButton* findButton(const char* id);
	UITextBox* findTextBox(const char* id);

	virtual bool pressed(const char* elementId) const;
	virtual bool released(const char* elementId) const;

	UIButton* getButton(const char* elementId);
	UISlider* getSlider(const char* elementId);

private:
	// reference to all the elements, ordered for rendering
	ScreenLayers mScreenLayers;
	// map of interactables for easy fetching by id
	std::unordered_map<StringBuffer32, UIElement*> mInteractables;

public:
	u32 mFlags;


	// Unused state override functions
private:
	void HandleInput() override { }
	void FastUpdate(float dt) override { }
	void Update(float dt) { };
};



class NullScreen : public Screen
{
public:
	NullScreen() { }

	void init() { }
	void Update() { }
	void exit() { }
	void render() { }
	UIScreen::Type type() { return UIScreen::Type::None; }

	bool pressed(const char* button) const override { return false; }
	bool released(const char* button) const override { return false; }
};