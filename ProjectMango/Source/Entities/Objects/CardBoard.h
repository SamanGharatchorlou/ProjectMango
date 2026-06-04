#pragma once


void ClearBoard();
void PopulateBoard(int rows, int columns);
void RestockTriggeredCards();

bool TriggerCard(ECS::Entity card, ECS::Entity owner);
void DiscardCard(ECS::Entity entity);