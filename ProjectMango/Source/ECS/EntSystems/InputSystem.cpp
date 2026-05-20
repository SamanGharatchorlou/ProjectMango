#include "pch.h"
#include "InputSystem.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Input/InputManager.h"
#include "Input/Cursor.h"
#include "Game/FrameRateController.h"
#include "Core/Helpers.h"
// todo: remove
#include "Entities/Factory/EntityBuilder.h"

void SetupButtonActionBindings(std::unordered_map<BasicString, std::function<void(ECS::Entity)>>& button_bindings);

namespace ECS
{
	std::unordered_map<BasicString, std::function<void(Entity)>> s_buttonActionBindings;

	// setup all text bindings
	void InputSystem::Init()
	{
		SetupButtonActionBindings(s_buttonActionBindings);
	}

	void InputSystem::Update(float dt)
	{
		const UICursor* cursor = UICursor::Get();
		if(!cursor)
			return;

		InputManager* input = InputManager::Get();
		const bool left_select = input->isCursorPressed(Cursor::ButtonType::Left);
			
		const FrameRateController& frc = FrameRateController::Get();
		const int frame_count = frc.frameCount;
		const VectorF cursor_pos = cursor->Position();

		if(left_select)
		{
			RectF cursor_rect;
			cursor_rect.SetSize(cursor->cursor->size() * 3.0f);
			cursor_rect.SetCenter(cursor_pos);

			int number = Maths::randomNumberBetween(0, 2);
			Entity vfx = CreateVFX( number == 0 ? "Hit1" : "Hit2", cursor_rect);
			Sprite& sprite = GetComponentRef(Sprite, vfx);
			sprite.params.rotation = (float)Maths::randomNumberBetween(0, 360);
			sprite.params.colourMod.setOpacity(0.7f);
		}

		for (Entity entity : entities)
		{
			if(UIButton* ui_button = GetComponent(UIButton, entity))
			{
				if(left_select)
				{
					Transform& transform = GetComponentRef(Transform, entity);
					
					if( Contains(transform.GetRect(), cursor_pos) )
					{
						ui_button->lastPressedFrameCount = frame_count;
						ui_button->toggle = !ui_button->toggle;
					}
				}

				if( ui_button->IsPressed() )
				{
					auto iter = s_buttonActionBindings.find(ui_button->callback.c_str());
					if(iter != s_buttonActionBindings.end())
						iter->second(ui_button->entity);
				}
			}
		}
	}
}