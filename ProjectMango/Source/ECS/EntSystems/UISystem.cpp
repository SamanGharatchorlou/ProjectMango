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

void SetupTextUIBindings(std::unordered_map<BasicString, std::function<BasicString()>>& text_bindings);
void SetupButtonUIBindings(std::unordered_map<BasicString, std::function<void(ECS::Entity)>>& button_bindings);

namespace ECS
{
	std::unordered_map<BasicString, std::function<BasicString()>> s_textBindings;
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
				if( !text->UID.empty() )
				{
					auto iter = s_textBindings.find(text->UID);
					if(iter != s_textBindings.end())
					{
						BasicString new_text = iter->second();
						if( text->font.text != new_text )
						{
							text->SetText(new_text.c_str());
						}
					}
				}
			}
			
			// update button visual e.g. make it green
			if(UIButton* button = GetComponent(UIButton, entity))
			{
				if( !button->UID.empty() )
				{
					auto iter = s_buttonBindings.find(button->UID);
					if(iter != s_buttonBindings.end())
					{
						iter->second(entity);
					}
				}
			}
			
			// show/hide checkbox tick
			if(UICheckbox* check_box = GetComponent(UICheckbox, entity))
			{
				Entity check = GetFirstChild(entity);
				Sprite check_sprite = GetComponentRef(Sprite, check);
				check_sprite.disabled = !check_box->isOn;
			}
		}

	}
}