#include "pch.h"
#include "PopupScreen.h"

#include "UI/ScreenController.h"
#include "UI/UIManager.h"
#include "Game/SystemStateManager.h"

#include "UI/Elements/UITextBox.h"
#include "UI/Elements/UIButton.h"

#include "Input/InputManager.h"
#include "System/Files/TextFileReader.h"


void PopupScreen::build(const char* title, const char* body)
{
	setTitle(title);
	setBody(body);
}


void PopupScreen::HandleInput(InputManager* input)
{
	if (input->isReleased(Button::Enter) || input->isReleased(Button::Esc))
	{
		exitPopup();
	}
}


void PopupScreen::Update()
{
	if (released("OkButton"))
	{
		exitPopup();
	}
}


void PopupScreen::setMainText(const char* textFile)
{
	const BasicString filePath = FileManager::Get()->findFile(FileManager::Config_Menus, textFile);
	TextFileReader reader(filePath.c_str());

	BasicString text("");
	reader.readText(text);

	UIElement* element = find("PopupText");
	UITextBox* textBox = static_cast<UITextBox*>(element);
	textBox->setText(text);
	textBox->autoSizeWrapFont();
}



void PopupScreen::setTitle(const char* title)
{
	UIElement* element = find("Title");
	UITextBox* textBox = static_cast<UITextBox*>(element);
	textBox->setText(title);
	textBox->align();
}

void PopupScreen::setBody(const char* body)
{
	UIElement* element = find("Body");
	UITextBox* textBox = static_cast<UITextBox*>(element);
	textBox->setText(body);
	textBox->autoSizeWrapFont();
}


void PopupScreen::exitPopup()
{
	//GameData::Get().uiManager->controller()->popScreen();
	//GameData::Get().systemStateManager->popState();
	//getButton("OkButton")->setActive(false);
}