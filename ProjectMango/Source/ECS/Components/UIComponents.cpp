#include "pch.h"
#include "UIComponents.h"

#include "ECS/EntityCoordinator.h"
#include "Game/FrameRateController.h"
#include "Input/Cursor.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/SpacialComponents.h"

namespace ECS
{
	// UICursor
	// ------------------------------------------------------------------
	UICursor::UICursor() : cursor(nullptr) { }

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
	UIButton::UIButton() : lastPressedFrameCount(-100), toggle(false) { }
	
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
		// default to white
		SetColour(SColour::White);
	}

	void UIText::SetText(const char* _text) 
	{ 
		text = _text;

		font.SetText(text.c_str()); 

		if(center)
			SetRenderOffsetToCenter();
	}
	
	void UIText::SetColour(SColour scolour) 
	{ 
		font.SetColour(text.c_str(), scolour.toSDL());
	}

		
	void UIText::SetSize(int ptSize) 
	{ 
		font.SetSize(text.c_str(), ptSize);

		if(center)
			SetRenderOffsetToCenter();
	}

	void UIText::FitToSize(VectorF size)
	{
		int pt_size = 0;
		font.GetPtSizeForArea(text.c_str(), size, pt_size);

		if(pt_size != 0)
		{
			SetSize(pt_size);
		}
	}

	void UIText::SetRenderOffsetToCenter()
	{
		const Transform& transform = GetComponentRef(Transform, entity);

		renderOffset = VectorF(0,0);
		renderOffset = transform.size * 0.5f - font.GetSize().toFloat() * 0.5f;
	}

}