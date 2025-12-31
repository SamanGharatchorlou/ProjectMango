#include "pch.h"
#include "ResourceBank.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"

using namespace ECS;

struct FactionResources
{
	Entity coins[Colour::Count];
	Entity cardPower[Colour::Count];
};

FactionResources s_bank[Faction::Count];
//
//FactionResources& GetResources(u32 faction_team)
//{
//	return s_bank[faction_team];
//}

void RegisterCoinResource(Entity entity, u32 faction)
{
	CoinStack& coin_stack = GetComponentRef(CoinStack, entity);
	s_bank[faction].coins[coin_stack.colourType] = entity;
}

void RegisterCardResource(Entity entity, u32 faction)
{
	CoinStack& coin_stack = GetComponentRef(CoinStack, entity);
	s_bank[faction].cardPower[coin_stack.colourType] = entity;
}


ECS::CoinStack& GetCoinStack(u32 faction, u32 colour )
{
	Entity entity = s_bank[faction].coins[colour];

#if DEBUG_MODE
	BasicString faction_name;
	Faction::DebugGetFactionName(faction, faction_name);
	Colour::Type colour_type = (Colour::Type)colour;
	ASSERT(entity != 0, "Coin stack for faction %s - %s has not been registered", faction_name.c_str(), Colour::s_typeToString.at(colour_type) );
#endif

	return GetComponentRef(CoinStack, entity);
}