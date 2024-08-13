#pragma once

#include "Core/Events/Observer.h"
#include "ScreenController.h"
#include "Graphics/Renderable.h"
#if UI_EDITOR
#include "Debugging/UIEditor.h"
#endif

class InputManager;
class Cursor;
class Screen;

class UIManager : public Observer, public Renderable
{
public:
	UIManager();
	~UIManager();

	void init();
	void preLoad();
	void load();
	void clearScreens();

	void initCursor(Cursor* cursor);
	void setCursorTexture(Texture* texture);

	void handleInput(InputManager* input);
	void update();
	void render() override;

	void handleEvent(EventData& data) override;
	
	template<class T>
	T* getScreen(UIScreen::Type type) { return static_cast<T*>(mScreens[type]); }
	Screen* getScreen(UIScreen::Type type);
	Screen* getActiveScreen();

	ScreenController* controller() { return &mController; }

private:
	ScreenController mController;
	Screen* mScreens[UIScreen::Type::Count];
	Cursor* mCursor;

#if UI_EDITOR
	UIEditor mEditor;
#endif
};