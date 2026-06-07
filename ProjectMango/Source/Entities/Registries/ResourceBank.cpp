#include "pch.h"
#include "ResourceBank.h"

#include "ECS/Components/GameComponents.h"
#include "ECS/EntityCoordinator.h"

using namespace ECS;

Entity s_globalCoinBank[Colour::Count];

void RegisterCoinResource(Entity entity)
{
	CoinStack& coin_stack = GetComponentRef(CoinStack, entity);
	s_globalCoinBank[coin_stack.colourType] = entity;
}

ECS::CoinStack& GetGlobalCoinBank(u32 colour )
{
	Entity entity = s_globalCoinBank[colour];
	return GetComponentRef(CoinStack, entity);
}

void TakeCoinsFromStack(ECS::Entity entity, u32 colour, int& amount)
{
	Inventory* inventory = GetComponent(Inventory, entity);
	if (!inventory)
	{
		amount = 0;
		return;
	}

	// take coins from the stack
	CoinStack& cs = GetGlobalCoinBank(colour);

	// we can only take coins that are available
	amount = Maths::Min(amount, cs.remaining);
	cs.remaining -= amount;
	inventory->coins[colour] += amount;
}

void ReturnCoinsToStack(Entity entity, u32 colour, int& amount)
{
	Inventory* inventory = GetComponent(Inventory, entity);
	if (!inventory)
	{
		amount = 0;
		return;
	}

	// return coins to the stack	
	CoinStack& cs = GetGlobalCoinBank((u32)colour);

	amount = Maths::Min(amount, inventory->coins[colour]);
	cs.remaining += amount;
	inventory->coins[colour] -= amount;
}