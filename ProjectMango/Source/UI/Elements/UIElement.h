#pragma once

#include "Core/TypeDefs.h"

class InputManager;

enum class UIType
{
	None,

	Element,
	Box,
	TextBox,
	Button,
	Switch,
	Slider
};

/* Base element for all UI components */
class UIElement
{
public:
	UIElement() : mParent(nullptr), mShow(false) { }
	UIElement(const StringMap32& attributes);
	virtual ~UIElement() { };

	virtual bool handleInput(InputManager* input) { return false; };
	virtual void render() { };

	void fill(const StringMap32& attributes);

	const char* id() const { return mId.c_str(); }

	void setRect(RectF rect);
	RectF rect() const { return mRect; }

	void setParernt(UIElement* parent) { mParent = parent; }
	const UIElement* parent() const { return mParent; }

	void addChild(UIElement* child) { mChildren.push_back(child); }
	std::vector<UIElement*> children() const { return mChildren; }

	virtual UIType type() const { return UIType::Element; }

	bool IsInteractable() const
	{
		return type() == UIType::Button ||
			type() == UIType::Slider ||
			type() == UIType::Switch;
	}

	void setVisibility(bool visible) { mShow = visible; }
	bool isVisible() const { return mShow; }

protected:
	StringBuffer32 mId;
	RectF mRect;

	const UIElement* mParent;
	std::vector<UIElement*> mChildren;

	bool mShow;
};

static UIType GetUIType(const char* type)
{
	if (StringCompare(type, "Element"))
		return UIType::Element;
	else if (StringCompare(type, "Box"))
		return UIType::Box;
	else if (StringCompare(type, "TextBox"))
		return UIType::TextBox;
	else if (StringCompare(type, "Button"))
		return UIType::Button;
	else if (StringCompare(type, "Switch"))
		return UIType::Switch;
	else if (StringCompare(type, "Slider"))
		return UIType::Slider;

	DebugPrint(Warning, "'%s' is not a valid UIType");
	return UIType::None;
}