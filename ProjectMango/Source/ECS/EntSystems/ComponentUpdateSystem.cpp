#include "pch.h"
#include "ComponentUpdateSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/UIComponents.h"

namespace ECS
{
	void ComponentUpdateSystem::Update(float dt)
	{
		for (Entity entity : entities)
		{
			if(Spawner* spawner = GetComponent(Spawner, entity))
			{
				spawner->Update();
			}
			if(Door* door = GetComponent(Door, entity))
			{
				door->Update();
			}			
			// dont think this is running, need to just fix what runs in this function
			if (Pickup* pick_up = GetComponent(Pickup, entity))
			{
				pick_up->Update();
			}
			if (DeathScentence* ds = GetComponent(DeathScentence, entity))
			{
				ds->Update(dt);
			}
			if (Arm* arm = GetComponent(Arm, entity))
			{
				arm->Update();
			}			
			if (Inventory* inventory = GetComponent(Inventory, entity))
			{
				ComponentArray<CoinStack>& coin_stacks =  GetAllComponents(CoinStack);
				for( auto iter = coin_stacks.entityToComponent.begin(); iter != coin_stacks.entityToComponent.end(); iter++ )
				{
					CoinStack& coin_stack = coin_stacks.GetComponentByIndex(iter->second);
					if(UIButton* button = GetComponent(UIButton, coin_stack.entity))
					{
						// take a coin from the coin stack
						if(button->IsPressed())
						{
							if(coin_stack.remaining > 0)
							{
								// remove from stack
								coin_stack.remaining--;

								// add to inventory
								//inventory->coins[coin_stack.colourType]++;
								inventory->SetCoinAmount(coin_stack.colourType, inventory->coins[coin_stack.colourType] + 1 );
							}
						}
					}
				}

				for( u32 i = 0; i < CoinStack::Count; i++ )
				{
					if(inventory->coinDisplay[i] == EntityInvalid)
					{

					}
				}
			}
		}
	}
}
	