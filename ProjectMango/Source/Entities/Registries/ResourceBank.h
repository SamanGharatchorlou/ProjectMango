#pragma once

namespace ECS { struct CoinStack; }

void RegisterCoinResource(ECS::Entity entity);

ECS::CoinStack& GetGlobalCoinBank(u32 colour );

void TakeCoinsFromStack(ECS::Entity entity, u32 colour, int& amount);
void ReturnCoinsToStack(ECS::Entity entity, u32 colour, int& amount);