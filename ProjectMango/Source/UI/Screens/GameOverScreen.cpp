#include "pch.h"
#include "GameOverScreen.h"

#include "Game/SystemStateManager.h"
#include "UI/Elements/UITextBox.h"

void GameOverScreen::Init()
{
	setTextBox("Kills", mKills);
	setTextBox("Score", mScore);
	setTextBox("Map Level", mMapLevel);
}


void GameOverScreen::Update()
{	
	if (released("ContinueButton"))
	{
		GameData::Get().systemStateManager->restart();
	}
}


void GameOverScreen::setTextBox(const char* textBoxName, int value)
{
	UITextBox* textBox = findTextBox(textBoxName);

	StringBuffer64 buffer;
	snprintf(buffer.buffer(), buffer.bufferLength(), "%s: %d", textBoxName, value);

	textBox->setText(buffer.c_str());
	textBox->align();
}
