#include "pch.h"
#include "Screen.h"

#include "Input/InputManager.h"
#include "Audio/AudioManager.h"
#include "UI/ScreenLayers.h"

#include "UI/Elements/UISwitch.h"
#include "UI/Elements/UIButton.h"
#include "UI/Elements/UITextBox.h"
#include "UI/Elements/UISlider.h"


Screen::~Screen()
{
	Elements elements = mScreenLayers.elementList();

	for (UIElement*& element : elements)
	{
		delete element;
		element = nullptr;
	}
}

void Screen::add(const ScreenLayer& layer)
{
	mScreenLayers.mLayers.push_back(layer);
	for (u32 i = 0; i < layer.elements.size(); i++)
	{
		UIElement* element = layer.elements[i];
		if (element->IsInteractable())
			mInteractables[element->id()] = element;
	}
}

void Screen::add(const ScreenLayers& layers)
{
	for (int i = 0; i < layers.mLayers.size(); i++)
	{
		add(layers.mLayers[i]);
	}
}

void Screen::updateInputs(InputManager* input)
{
	bool hasPressed = false;
	std::unordered_map<StringBuffer32, UIElement*>::iterator iter;
	for (iter = mInteractables.begin(); iter != mInteractables.end(); iter++)
	{
		if ((iter->second)->handleInput(input))
			hasPressed = true;
	}

#if !DISABLE_UI_AUDIO
	if (hasPressed)
	{
		AudioManager::Get()->push(AudioEvent(AudioEvent::Play, "ButtonClick", nullptr));
	}
	else if(input->isCursorPressed(Cursor::Left))
	{
		AudioManager::Get()->push(AudioEvent(AudioEvent::Play, "MouseClick", nullptr));

	}
#endif
}


void Screen::render() 
{
	for (const ScreenLayer& layer : mScreenLayers.mLayers)
	{
		for (UIElement* element : layer.elements)
		{
			element->render();
		}
	}
}


UIElement* Screen::find(const char* id)
{
	return mScreenLayers.find(id);
}


UIButton* Screen::findButton(const char* id)
{
	UIElement* element = mScreenLayers.find(id);

	if (element->type() == UIType::Button)
	{
		UIButton* button = static_cast<UIButton*>(element);
		return button;
	}
	else
	{
		DebugPrint(Warning, "Attemping to use findButton on a none button element, ID: '%s'", id);
		return nullptr;
	}
}

UITextBox* Screen::findTextBox(const char* id)
{
	UIElement* element = mScreenLayers.find(id);

	if (element->type() == UIType::TextBox)
	{
		UITextBox* textBox = static_cast<UITextBox*>(element);
		return textBox;
	}
	else
	{
		DebugPrint(Warning, "Attemping to use findTextBox on a none textbox element, ID: '%s'", id);
		return nullptr;
	}
}

static bool CheckExists(const std::unordered_map<StringBuffer32, UIElement*>& map, const char* id, const char* type)
{
	if (map.count(id) == 0)
	{
		DebugPrint(Warning, "No %s %s exists", type, id);
		return false;
	}

	return true;
}

bool Screen::pressed(const char* elementId) const
{
#if DEBUG_MODE
	if (!CheckExists(mInteractables, elementId, "button"))
	{
		return false;
	}
#endif

	UIElement* element = mInteractables.at(elementId);
	return static_cast<UIButton*>(element)->isPressed();
}

bool Screen::released(const char* elementId) const
{
#if DEBUG_MODE
	if (!CheckExists(mInteractables, elementId, "button"))
	{
		return false;
	}
#endif

	UIElement* element = mInteractables.at(elementId);
	return static_cast<UIButton*>(element)->isReleased();
}


UISlider* Screen::getSlider(const char* elementId)
{
#if DEBUG_MODE
	if (!CheckExists(mInteractables, elementId, "slider"))
	{
		return nullptr;
	}
#endif

	UIElement* element = mInteractables.at(elementId);
	return static_cast<UISlider*>(element);
}

UIButton* Screen::getButton(const char* elementId)
{
#if DEBUG_MODE
	if (!CheckExists(mInteractables, elementId, "button"))
	{
		return nullptr;
	}
#endif

	UIElement* element = mInteractables.at(elementId);
	return static_cast<UIButton*>(element);
}