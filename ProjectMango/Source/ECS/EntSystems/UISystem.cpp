#include "pch.h"
#include "UISystem.h"

#include "ECS/Components/UIComponents.h"
#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "Input/InputManager.h"
#include "Input/Cursor.h"
#include "Game/Camera/Camera.h"
#include "ECS/Components/Biome.h"
#include "System/Window.h"
#include "Game/FrameRateController.h"
#include "Core/Helpers.h"

namespace ECS
{
	void UISystem::Update(float dt)
	{
		const UICursor* cursor = UICursor::Get();
		if(!cursor)
			return;

		for (Entity entity : entities)
		{
			if(UICursor* ui_cursor = GetComponent(UICursor, entity))
			{
				if (ui_cursor->cursor)
				{
					Camera* camera = Camera::Get();
					VectorF camera_offset = camera->GetRect().TopLeft();

					// handle window scaling
					int width = -1;
					int height = -1;
					SDL_GetWindowSize(GameData::Get().window->get(), &width, &height);
					const VectorF real_window_size = VectorF((float)width, (float)height);
					const VectorF fake_window_size = GameData::Get().window->size();
					const float render_scale = real_window_size.x / fake_window_size.x;

					Transform& transform = GetComponentRef(Transform, entity);
					VectorF map_position = (ui_cursor->cursor->position() / render_scale) + camera_offset;
					transform.SetWorldPosition(map_position);
				}
			}
			
			InputManager* input = InputManager::Get();
			const bool left_select = input->isCursorPressed(Cursor::ButtonType::Left);
			
			const FrameRateController& frc = FrameRateController::Get();
			const int frame_count = frc.frameCount;

			const UICursor* cursor = UICursor::Get();
			VectorF cursor_pos = cursor->Position();

			if(UIButton* ui_button = GetComponent(UIButton, entity))
			{
				if(left_select)
				{
					Transform& transform = GetComponentRef(Transform, entity);
					
					if( Contains(transform.GetRect(), cursor_pos) )
						ui_button->lastPressedFrameCount = frame_count;
				}
			}
		}
	}
}