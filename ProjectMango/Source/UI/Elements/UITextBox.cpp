#include "pch.h"
#include "UITextBox.h"

UITextBox::UITextBox(const StringMap32& attributes, const BasicString& text) : UIBox(attributes)
{
	initText(attributes, text);

	StringBuffer32 aligment = attributes.contains("align") ? attributes.at("align") : "";
	setAlignment(aligment);
}

void UITextBox::render()
{
	mText.render(mRect.TopLeft());
	UIBox::render();
}

void UITextBox::autoSizeFont()
{
	mText.autoSize(mRect.Size());
}


void UITextBox::autoSizeWrapFont()
{
	mText.autoSizeWrap(mRect.Size());
}

void UITextBox::align()
{
	if (mAlignment == Alignment::Center)
	{
		float textWidth = mText.size().x;
		float rectWidth = mRect.Width();

		float xOffset = (rectWidth - textWidth) / 2;
		mText.setOffset(VectorF(xOffset, 0.0f));
	}
	else if (mAlignment == Alignment::Left || mAlignment == Alignment::Right)
	{
		DebugPrint(Warning, "Unimplemented");
	}
}

void UITextBox::autoSizeRectToText()
{
	mRect.SetSize(mText.size());
	align();
}


// -- Private Functions -- //
void UITextBox::setAlignment(const StringBuffer32& alignment)
{
	if (alignment == "Center")
		mAlignment = Alignment::Center;
	else if (alignment == "Left")
		mAlignment = Alignment::Left;
	else if (alignment == "Right")
		mAlignment = Alignment::Right;
	else
		mAlignment = Alignment::None;

	align();
}

void UITextBox::initText(const StringMap32& attributes, const BasicString& text)
{
	StringBuffer32 font = attributes.contains("font") ? attributes.at("font") : "default";
	int ptSize = attributes.contains("ptSize") ? attributes.getInt("ptSize") : 0;

	SDL_Color colour;
	if (attributes.contains("r"))
	{
		colour = {
		(Uint8)attributes.getInt("r"),
		(Uint8)attributes.getInt("g"),
		(Uint8)attributes.getInt("b")
		};
	}
	else // dafult to white
	{
		colour = { 255, 255, 255 };
	}

	mText.init(text.c_str(), font, ptSize, colour);
}
