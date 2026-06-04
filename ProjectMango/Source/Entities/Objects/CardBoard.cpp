#include "pch.h"
#include "CardBoard.h"

#include "Entities/Registries/CardRegistry.h"
#include "Entities/Registries/SpellRegistry.h"
#include "ECS/Components/IncludeComponents.h"
#include "Entities/Factory/ComponentAssembler.h"
#include "ECS/EntityCoordinator.h"
#include "System/Window.h"
#include "Core/Helpers.h"

using namespace ECS;

void ClearBoard()
{
	CardBoard* board = GetOnlyComponent(CardBoard);
	if (!board)
		return;

	Grid<Entity>& cards = board->cards;
	for (u32 y = 0; y < board->cards.rows(); y++)
	{
		for (u32 x = 0; x < board->cards.colums(); x++)
		{
			Entity entity = cards.get(VectorI(x, y));
			DestroyEntityAndChildren(entity);
		}
	}

	for (u32 i = 0; i < Card::c_tiers; i++)
	{
		board->drawPile[i].clear();
		board->discardPile[i].clear();
	}

	board->triggeredCards.clear();
}

bool TriggerCard(ECS::Entity card_entity, ECS::Entity owner)
{
	bool triggered_spell = false;

	CardBoard* board = GetOnlyComponent(CardBoard);
	if (!board)
		return triggered_spell;
	

	Card& card = GetComponentRef(Card, card_entity);
	board->triggeredCards.push_back(card.boardIndex);

	Entity target = Faction::GetTarget(owner);
	if (target != EntityInvalid)
	{
		Entity spell_entity = SpellRegistry::CreateSpell(card.spell.c_str(), target);
		triggered_spell = spell_entity != EntityInvalid;
	}

	DiscardCard(card_entity);

	return triggered_spell;
}

void DiscardCard(Entity entity)
{
	CardBoard* board = GetOnlyComponent(CardBoard);
	if (!board)
		return;

	// place into discard pile
	Card& card = GetComponentRef(Card, entity);
	board->discardPile[card.tier].push_back(card.registryIndex);

	// destroy the card
	DestroyEntityAndChildren(entity);
}

static bool DrawRandomCard(CardBoard& board, int tier, DeckCard& out_dc)
{
	std::vector<DeckCard>& draw_pile = board.drawPile[tier];

	if (draw_pile.size() > 0)
	{
		// weighted random pick
		int total = 0;
		for (const DeckCard& dc : draw_pile)
			total += dc.weight;

		int roll = Maths::randomNumberBetween(0, total);
		int cumulative = 0.0f;
		int chosen_index = (int)draw_pile.size() - 1;
		for (u32 i = 0; i < draw_pile.size(); i++)
		{
			cumulative += draw_pile[i].weight;
			if (roll < cumulative)
			{
				chosen_index = i;
				break;
			}
		}

		out_dc = draw_pile[chosen_index];

		// remove from the draw pile
		Erase(draw_pile, out_dc);

		return true;
	}

	return false;
}

static VectorF CardPosFromIndex(const CardBoard& board, VectorI index)
{
	RectF rect = GetComponentRef(Transform, board.entity).GetRect();
	float x_spacing = rect.Width() / (board.cards.colums());
	float y_spacing = -rect.Height() / (board.cards.rows());
	VectorF offset = rect.BotLeft() + VectorF(x_spacing, y_spacing) * 0.5f;

	VectorF world_pos = offset + index.toFloat() * VectorF(x_spacing, y_spacing);
	return world_pos;
}

static void CreateNewCard(CardBoard& board, VectorI index)
{
	int tier = index.y;

	char buffer[32];
	snprintf(buffer, 32, "card %d,%d", index.x, tier);

	DeckCard random_card;
	DrawRandomCard(board, tier, random_card);

	VectorF world_pos = CardPosFromIndex(board, index);
	Entity card_entity = CardRegistry::CreateCard(buffer, world_pos, random_card);

	Card& card = GetComponentRef(Card, card_entity);
	card.boardIndex = index;

	board.cards[index] = card_entity;

	TriggerGameEvent(GameEvent::CardDrawn, card_entity);
}

void RestockTriggeredCards()
{
	CardBoard* board = GetOnlyComponent(CardBoard);
	if (!board)
		return;

	for (u32 i = 0; i < board->triggeredCards.size(); i++)
	{
		VectorI index = board->triggeredCards[i];
		CreateNewCard(*board, index);
	}

	board->triggeredCards.clear();
}

void PopulateBoard(int rows, int columns)
{
	ClearBoard();

	CardBoard* board = GetOnlyComponent(CardBoard);
	if (!board)
		return;

   	board->cards.set( VectorI(rows, columns), EntityInvalid);
	CardRegistry::PopulateDrawPiles(board->drawPile, Card::c_tiers);

	TriggerGameEvent(GameEvent::DrawPileBuilt, board->entity);

	for (u32 y = 0; y < board->cards.rows(); y++)
	{
		for (u32 x = 0; x < board->cards.colums(); x++)
		{
			VectorI index(x, y);
			CreateNewCard(*board, index);
		}
	}
}
