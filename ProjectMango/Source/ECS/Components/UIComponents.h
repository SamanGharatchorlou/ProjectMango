#pragma once

#include "UI/Text/Font.h"
#include "ECS/EntityCommon.h"

class Cursor;

namespace ECS
{
	struct EntityMetaData;

	struct UICursor
	{
		COMPONENT_TYPE(UICursor)

		Cursor* cursor;
		
		UICursor();

		static UICursor* Get();

		VectorF Position() const;
	};

	struct UIButton
	{
		COMPONENT_TYPE(UIButton);

		static constexpr const char* kRequirement = "button_callback";
		
		BasicString callback;

		int lastPressedFrameCount;

		bool toggle;
		
		UIButton();
		void Init(const EntityMetaData& emd);
		void Serialise(EntityMetaData& out_emd) const;

		bool IsPressed(int frame_buffer = 0) const;
	};

	struct UIText
	{
		COMPONENT_TYPE(UIText);

		static constexpr const char* kRequirement = "text";
		void Init(const EntityMetaData& emd);
		void Serialise(EntityMetaData& out_emd) const;

		BasicString callback;

		BasicString text;
		Font font;

		// useful when part of another thing that has a sprite and we want to draw center to it
		VectorF renderOffset;
		bool center;
		
		UIText();

		void SetText(const char* text);
		void SetColour(SColour scolour);

		void SetSize(int ptsize);
		void FitToSize(VectorF size);

		void UpdateRenderOffset();

		void Render();
	};

	struct UIIntentIcon
	{
		COMPONENT_TYPE(UIIntentIcon);

		struct Display
		{
			BasicString icon;
			u32 intent;
		};

		std::vector<Display> displays;
	};
}