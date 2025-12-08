#include "pch.h"
#include "UIComponents.h"

#include "ECS/EntityCoordinator.h"
#include "Game/FrameRateController.h"
#include "Input/Cursor.h"

namespace ECS
{
	// UICursor
	// ------------------------------------------------------------------
	UICursor::UICursor() : cursor(nullptr) { }

	UICursor::~UICursor() { }

	UICursor* UICursor::Get()
	{
		ComponentArray<UICursor>& cursors = GetAllComponents(UICursor);
		if (cursors.Count() > 0)
		{
			auto front_index = cursors.entityToComponent.begin();
			return &cursors.GetComponentByIndex(front_index->second);
		}

		return nullptr;
	}

	VectorF UICursor::Position() const
	{
		return cursor->position();
	}
	

	// UIButton
	// ------------------------------------------------------------------
	UIButton::UIButton() : lastPressedFrameCount(-100) { }
	
	bool UIButton::IsPressed(int frame_buffer) const
	{
		const FrameRateController& frc = FrameRateController::Get();
		const int frame_count = frc.frameCount;

		return (lastPressedFrameCount + frame_buffer) >= frame_count;
	}
	

	// UIButton
	// ------------------------------------------------------------------
	UIText::UIText()
	{
		BasicString path = FileManager::Get()->findFile(FileManager::Font, c_defaultFont);
		font.LoadFromFile(path.c_str(), c_defaultFontSize);

		// default to white
		SetColour(SColour::White);
	}

	void UIText::SetText(const char* _text) 
	{ 
		//text = _text;
		font.SetText(_text); 
	}
	
	void UIText::SetColour(SColour::Enum _colour) 
	{ 
		font.colour = SColour(_colour).toSDL();
		SetText(font.text.c_str());
	}
}