#pragma once

namespace ECS { struct Card; }

void InitBoardConfigurations();

void ClearBoard();
void SetupBoard(int biome_index);
void RestockDiscardedCards();

bool TriggerCard(ECS::Entity entity, ECS::Entity owner);
void DiscardCard(ECS::Entity entity);
void RedrawCard(ECS::Entity entity);