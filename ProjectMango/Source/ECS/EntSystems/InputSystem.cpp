#include "pch.h"
#include "InputSystem.h"

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
#include "Entities/Player/PlayerCharacter.h"

namespace ECS
{
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

		for (Entity entity : entities)
		{
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

		// handle coin stacks
		ComponentArray<CoinStack>& coin_stacks =  GetAllComponents(CoinStack);
		for( auto iter = coin_stacks.entityToComponent.begin(); iter != coin_stacks.entityToComponent.end(); iter++ )
		{
			CoinStack& coin_stack = coin_stacks.GetComponentByIndex(iter->second);
			if(UIButton* button = GetComponent(UIButton, coin_stack.entity))
			{
				// take a coin from the coin stack
				if(button->IsPressed())
				{
					ActionRequest& action_request = AddComponent(ActionRequest, Player::Get());
					action_request.request = ActionRequest::CollectCoin;
					action_request.target = coin_stack.entity;
				}
			}
		}

		// handle cards
		ComponentArray<Card>& cards =  GetAllComponents(Card);
		for( auto iter = cards.entityToComponent.begin(); iter != cards.entityToComponent.end(); iter++ )
		{
			Card& card = cards.GetComponentByIndex(iter->second);
			if(UIButton* button = GetComponent(UIButton, card.entity))
			{
				// take a coin from the coin stack
				if(button->IsPressed())
				{
					ActionRequest& action_request = AddComponent(ActionRequest, Player::Get());
					action_request.request = ActionRequest::AquireCard;
					action_request.target = card.entity;
				}
			}
		}
	}
}