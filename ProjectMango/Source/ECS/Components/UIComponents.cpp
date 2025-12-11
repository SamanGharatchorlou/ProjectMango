#include "pch.h"
#include "UIComponents.h"

#include "ECS/EntityCoordinator.h"
#include "Game/FrameRateController.h"
#include "Input/Cursor.h"
#include "ECS/Components/Components.h"

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
	UIText::UIText() : center(false)
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
		if(center)
			SetRenderOffsetToCenter();
	}
	
	void UIText::SetColour(SColour::Enum _colour) 
	{ 
		font.colour = SColour(_colour).toSDL();
		SetText(font.text.c_str());
	}

	void UIText::FitToSize(VectorF size)
	{
		int width = -1;
		int height = -1;

		int targetWidth = (int)size.x;
		int targetHeight = (int)size.y;

		int ptSize = font.GetPtSize();

		TTF_SizeText(font.ttfFont, font.text.c_str(), &width, &height);

		if (width > targetWidth || height > targetHeight)
		{
			while (width > targetWidth || height > targetHeight)
			{
				font.Resize(--ptSize);
				TTF_SizeText(font.ttfFont, font.text.c_str(), &width, &height);
			}
		}
		else
		{
			while (width < targetWidth && height < targetHeight)
			{
				font.Resize(++ptSize);
				TTF_SizeText(font.ttfFont, font.text.c_str(), &width, &height);
			}
		}

		font.SetText(font.text.c_str());
	}

	
	void UIText::SetRenderOffsetToCenter()
	{
		const Transform& transform = GetComponentRef(Transform, entity);

		renderOffset = VectorF(0,0);
		renderOffset = transform.size * 0.5f - font.GetSize().toFloat() * 0.5f;
	}
}