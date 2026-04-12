#pragma once

//FactionResources& GetResources(u32 faction_team);

namespace ECS { struct CoinStack; }

void RegisterCoinResource(ECS::Entity entity, u32 faction);
void RegisterCardResource(ECS::Entity entity, u32 faction);

ECS::CoinStack& GetCoinStack(u32 faction, u32 colour );