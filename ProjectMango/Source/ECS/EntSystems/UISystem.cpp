#include "pch.h"
#include "UISystem.h"

#include "ECS/Components/Components.h"
#include "ECS/Components/UIComponents.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/SystemManager.h"
#include "Entities/Player/PlayerCharacter.h"
#include "Game/Camera/Camera.h"
#include "Game/States/GameState.h"
#include "Game/SystemStateManager.h"
#include "Input/Cursor.h"
#include "System/Window.h"

void SetupTextBindings(std::unordered_map<BasicString, std::function<BasicString()>>& text_bindings);

namespace ECS
{
	std::unordered_map<BasicString, std::function<BasicString()>> s_textBindings;

	// setup all text bindings
	void UISystem::Init()
	{
		SetupTextBindings(s_textBindings);
	}

	void UISystem::Update(float dt)
	{
		// update cursor position
		if(UICursor* cursor = UICursor::Get())
		{
			if (cursor->cursor)
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

				Transform& transform = GetComponentRef(Transform, cursor->entity);
				VectorF map_position = (cursor->cursor->position() / render_scale) + camera_offset;
				transform.SetWorldPosition(map_position);
			}
		}

		for (Entity entity : entities)
		{
			UIText& text = GetComponentRef(UIText, entity);

			if(!text.fn)
			{
				if(!text.UID.empty() && s_textBindings.contains(text.UID))
				{
					text.fn = s_textBindings.at(text.UID);
				}
			}
			else
			{
				BasicString new_text = text.fn();
				if( text.font.text != new_text )
				{
					text.SetText(new_text.c_str());
				}
			}
		}

	}
}