#include "pch.h"
#include "UISystem.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/SystemManager.h"
#include "Game/Camera/Camera.h"
#include "Input/Cursor.h"
#include "System/Window.h"

void SetupTextUIBindings(std::unordered_map<BasicString, std::function<BasicString(ECS::Entity)>>& text_bindings);
void SetupButtonUIBindings(std::unordered_map<BasicString, std::function<void(ECS::Entity)>>& button_bindings);

namespace ECS
{
	std::unordered_map<BasicString, std::function<BasicString(Entity entity)>> s_textBindings;
	std::unordered_map<BasicString, std::function<void(ECS::Entity)>> s_buttonBindings;

	// setup all text bindings
	void UISystem::Init()
	{
		SetupTextUIBindings(s_textBindings);
		SetupButtonUIBindings(s_buttonBindings);
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
			// set ui text
			if(UIText* text = GetComponent(UIText, entity))
			{
				if( !text->callback.empty() )
				{
					auto iter = s_textBindings.find(text->callback);
					if(iter != s_textBindings.end())
					{
						const BasicString& new_text = iter->second(entity);
						if( text->text != new_text )
						{
							text->SetText(new_text.c_str());
						}
					}
				}
			}
			
			// update button visual e.g. make it green
			if(UIButton* button = GetComponent(UIButton, entity))
			{
				if( !button->callback.empty() )
				{
					auto iter = s_buttonBindings.find(button->callback);
					if(iter != s_buttonBindings.end())
					{
						iter->second(entity);
					}
				}
			}
		}
	}
}