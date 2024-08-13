#pragma once

#include "UIBox.h"
#include "UI/Text/Text.h"

class UITextBox : public UIBox
{
public:
	enum Alignment
	{
		None,
		Left,
		Right,
		Center
	};

	UITextBox(const StringMap32& attributes, const BasicString& text);
	UITextBox() { }
	virtual ~UITextBox() { }

	void setText(const BasicString& text) { mText.setText(text); }
	const BasicString& text() const { return mText.text(); }

	void setColour(SDL_Color colour) { mText.setColour(colour); }

	int fontSize() const { return mText.fontSize(); }
	void autoSizeFont();
	void autoSizeWrapFont();

	void autoSizeRectToText();
	VectorF textSize() const { return mText.size(); }

	void align();

	void render() override;

	virtual UIType type() const { return UIType::TextBox; }

	void initText(const StringMap32& attributes, const BasicString& text);
	void setAlignment(const StringBuffer32& alignment);

	Text mText;
	Alignment mAlignment;
};
