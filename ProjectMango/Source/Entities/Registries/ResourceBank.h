#pragma once

namespace ECS { struct CoinStack; }

void RegisterCoinResource(ECS::Entity entity);

ECS::CoinStack& GetGlobalCoinBank(u32 colour );