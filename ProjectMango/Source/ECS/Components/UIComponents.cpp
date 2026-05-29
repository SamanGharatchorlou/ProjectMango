#include "pch.h"
#include "UIComponents.h"

#include "ECS/EntityCoordinator.h"
#include "Game/FrameRateController.h"
#include "Input/Cursor.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/SpacialComponents.h"
#include "Core/Helpers.h"
#include "GameComponents.h"

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

	void UIButton::Init(const EntityMetaData& emd)
	{
		callback = emd.data.GetString(kRequirement);
	}

	void UIButton::Serialise(EntityMetaData& out_emd) const
	{
		if (callback.c_str())
			out_emd.data.AddString(kRequirement, callback.c_str());
	}

	bool UIButton::IsPressed(int frame_buffer) const
	{
		const FrameRateController& frc = FrameRateController::Get();
		const int frame_count = frc.frameCount;

		return (lastPressedFrameCount + frame_buffer) >= frame_count;
	}
	

	// UIText
	// ------------------------------------------------------------------
	UIText::UIText() : center(false)
	{
		// default to white
		SetColour(SColour::White);
	}

	void UIText::Init(const EntityMetaData& emd)
	{
		font.wrapped = emd.data.GetBool("wrapped");
		center = emd.data.GetBool("center_text");
		SetText(emd.data.GetString(kRequirement));
		SetSize(emd.data.GetInt("pt_size"));
		SetColour(emd.data.GetColour("colour"));

		callback = emd.data.GetString("text_callback");

		Colour::Type colour_type = (Colour::Type)emd.data.GetInt("colour_type", -1.0f);
		if (colour_type != -1)
		{
			Colour& colour = AddComponent(Colour, entity);
			colour.colour = colour_type;
		}
	}

	void UIText::Serialise(EntityMetaData& out_emd) const
	{
		if (text.c_str())
			out_emd.data.AddString(kRequirement, text.c_str());

		if (callback.c_str())
		{
			out_emd.data.AddString("text_callback", callback.c_str());

			if (!out_emd.data.Contains(kRequirement))
			{
				out_emd.data.AddString(kRequirement, "");
			}
		}

		int pt_size = (int)(RevertFromScreenSize((float)font.GetPtSize()) + 0.5f);
		out_emd.data.AddInt("pt_size", pt_size);
		out_emd.data.AddBool("center_text", center);
		out_emd.data.AddBool("wrapped", font.wrapped);
	}

	void UIText::SetText(const char* _text) 
	{ 
		text = _text;

		font.SetText(text.c_str());

		if (font.wrapped )
		{
			const Transform& transform = GetComponentRef(Transform, entity);
			font.width = (int)(transform.size.x + 0.5f);
		}

		UpdateRenderOffset();
	}
	
	void UIText::SetColour(SColour scolour) 
	{ 
		font.SetColour(text.c_str(), scolour.toSDL());
	}

	void UIText::SetSize(int ptSize) 
	{ 
		ptSize = (int)(AdjustToScreenSize((float)ptSize) + 0.5f);
		font.SetSize(text.c_str(), ptSize);
		UpdateRenderOffset();
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

	void UIText::UpdateRenderOffset()
	{
		const Transform& transform = GetComponentRef(Transform, entity);
		if (center)
		{
			renderOffset = transform.size * 0.5f - font.GetSize().toFloat() * 0.5f;
		}
		else
		{
			renderOffset = transform.size * 0.05f;
		}
	}

}