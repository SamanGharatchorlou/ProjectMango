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