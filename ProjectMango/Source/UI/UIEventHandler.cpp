#include "pch.h"
#include "UIEventHandler.h"

#include "Game/GameController.h"
#include "ScreenController.h"

#include "UIManager.h"
#include "Screens/Screen.h"
#include "Screens/GameOverScreen.h"

#include "Elements/UIElement.h"
#include "Elements/UIButton.h"
#include "Elements/UITextBox.h"
#include "Elements/UISlider.h"


void UIEventHandler::handleEvent(EventData& data)
{
	ScreenController* controller = GameData::Get().uiManager->controller();

	switch (data.eventType)
	{
	case Event::UpdateTextBox:
	{
		updateTextBox(controller->getActiveScreen(), static_cast<UpdateTextBoxEvent&>(data));
		break;
	}
	case Event::SetUIBar:
	{
		setUIbar(controller->getActiveScreen(), static_cast<SetUIBarEvent&>(data));
		break;
	}
	case Event::SetTextColour:
	{
		setTextColour(controller->getActiveScreen(), static_cast<SetTextColourEvent&>(data));
		break;
	}
	case Event::SetUISlider:
	{
		setSliderValue(controller->getActiveScreen(), static_cast<SetUISlider&>(data));
		break;
	}
	case Event::OpenPopup:
	{
		OpenPopupEvent& eventData = static_cast<OpenPopupEvent&>(data);
		controller->openPopup(eventData.mInfoID.c_str());
		break;
	}
	case Event::PlayerDead:
	{
		PlayerDeadEvent& eventData = static_cast<PlayerDeadEvent&>(data);
		if (eventData.mMapLevel >= 0)
		{
			Screen* screen = GameData::Get().uiManager->getScreen(UIScreen::Type::GameOver);
			GameOverScreen* gameOver = static_cast<GameOverScreen*>(screen);

			gameOver->mScore = eventData.mScore;
			gameOver->mKills = eventData.mKills;
			gameOver->mMapLevel = eventData.mMapLevel;
			
			GameData::Get().systemStateManager->addState(SystemStates::PauseState);

			controller->addScreen(UIScreen::Type::GameOver);
		}
		break;
	}

#if UI_EDITOR
	case Event::MoveUIElement:
	{
		moveElement(controller->getActiveScreen(), static_cast<EditUIRectEvent&>(data));
		break;
	}
	case Event::ChangeUIElementSize:
	{
		setElementSize(controller->getActiveScreen(), static_cast<EditUIRectEvent&>(data));
		break;
	}
#endif
	default:
		break;
	}
}


void UIEventHandler::setSliderValue(Screen* activeScreen, SetUISlider& eventData)
{
	UIElement* element = activeScreen->find(eventData.mId.c_str());
	if (element)
	{
		UISlider* slider = static_cast<UISlider*>(element);
		slider->setSliderValue(eventData.mValue);
	}
	else
	{
		DebugPrint(Warning, "Cannot find the slider, wrong screen? probably requesting during a popup");
	}

}


void UIEventHandler::updateTextBox(Screen* activeScreen, UpdateTextBoxEvent& eventData)
{
	UIElement* element = activeScreen->find(eventData.mId.c_str());

	if (element != nullptr && element->type() == UIType::TextBox)
	{
		UITextBox* text = static_cast<UITextBox*>(element);
		text->setText(eventData.mText.c_str());
	}
}


void UIEventHandler::setUIbar(Screen* activeScreen, SetUIBarEvent& eventData)
{
	UIElement* bar = activeScreen->find(eventData.mBar.c_str());
	UIElement* container = activeScreen->find(eventData.mBarContainer.c_str());

	if (bar != nullptr && bar->type() == UIType::Box &&
		container != nullptr && container->type() == UIType::Box)
	{
		UIBox* barBox = static_cast<UIBox*>(bar);
		UIBox* containerBox = static_cast<UIBox*>(container);

		RectF barRect = barBox->rect();
		RectF containerRect = containerBox->rect();

		barRect.SetWidth(containerRect.Width() * eventData.mPercentage);

		barBox->setRect(barRect);
	}
}


void UIEventHandler::setTextColour(Screen* activeScreen, SetTextColourEvent& eventData)
{
	UIElement* element = activeScreen->find(eventData.mId.c_str());

	if (element != nullptr && element->type() == UIType::TextBox)
	{
		UITextBox* textBox = static_cast<UITextBox*>(element);
		textBox->setColour(eventData.mColour);
	}
}


#if UI_EDITOR
void UIEventHandler::moveElement(Screen* activeScreen, EditUIRectEvent& eventData)
{
	UIElement* element = activeScreen->find(eventData.mId);

	if (element != nullptr)
	{
		VectorF movement = eventData.mChange;
		RectF rect = element->rect();
		rect = rect.Translate(movement);
		element->setRect(rect);
	}
}

void UIEventHandler::setElementSize(Screen* activeScreen, EditUIRectEvent& eventData)
{
	UIElement* element = activeScreen->find(eventData.mId);

	if (element != nullptr)
	{
		VectorF sizeChange = eventData.mChange;
		RectF rect = element->rect();
		rect.SetSize(rect.Size() + sizeChange);

		element->setRect(rect);
	}

}
#endif