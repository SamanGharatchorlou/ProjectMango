#include "pch.h"
#include "GameComponents.h"

#include "ECS/EntityCommon.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/UIComponents.h"
#include "Entities/CardRegistry.h"
#include "Entities/EntityBuilder.h"


namespace ECS
{
	// Inventory
	// ------------------------------------------------------------------
	Inventory::Inventory() { }
	
	void Inventory::GetCardPower(int array[], int size) const
	{
		memset(array, 0, sizeof(int) * size);
		for( u32 i = 0; i < cards.size(); i++ )
		{
			const ECS::Card* card = CardRegistry::LookupCard(cards[i]);
			for( int j = 0; j < size; j++ )
			{
				array[j] +=  card->power[j];
			}
		}
	}

	void Inventory::GetBuyingPower(int array[], int size) const
	{
		memset(array, 0, sizeof(int) * size);

		int card_power[Colour::Count] { 0 };
		GetCardPower(card_power, size);

		for( u32 i = 0; i < size; i++ )
		{
			array[i] = coins[i] + card_power[i];
		}
	}

	// Card
	// ------------------------------------------------------------------
	Card::Card() : points(0), tier(0) { }

	void Card::RegenerateChildDisplays()
	{
		DestroyChildren(entity);

		const Transform& transform = GetComponentRef(Transform, entity);
		const Sprite& sprite = GetComponentRef(Sprite, entity);

		int count = 0;
		for( u32 i = 0; i < Colour::Count; i++ )
		{
			if(cost[i] > 0)
			{
				Entity child_entity = CreateBasicObject("card_icon", VectorF(64,64));
				EntityData::SetParent(child_entity, entity);

				// where on the card is it placed
				int use_count = 0;
				if(count > 0)
					use_count = 1;
				if( count > 2)
					use_count *= 2;
				if(count % 2)
					use_count = -use_count;
			
				Transform& child_transform = GetComponentRef(Transform, child_entity);
				VectorF visible_size = child_transform.size * 0.8f;
				VectorF position = (transform.size - child_transform.size) * 0.5f;
				position += VectorF(0, child_transform.size.y * 0.8f) * (float)use_count;
				child_transform.SetLocalPosition( position );
			
				const StringBuffer32& colour_string = Colour::s_typeToString.at((Colour::Type)i);

				char buffer[32];
				snprintf(buffer, 32, "%s_gem", colour_string.c_str());

				Sprite& child_sprite = GetComponentRef(Sprite, child_entity);
				child_sprite.SetTexture(buffer);
				child_sprite.renderLayer = (RenderLayer)((int)sprite.renderLayer + 1);

				UIText& child_text_display = AddComponent(UIText, child_entity);

				BasicString text(cost[i]);
				child_text_display.SetText( text.c_str() );
				child_text_display.FitToSize(visible_size);
				child_text_display.SetRenderOffsetToCenter();

				count++;
			}
		}
	}


	// CoinStack
	// ------------------------------------------------------------------
	CoinStack::CoinStack() : capacity(0), remaining(0)/*, colour(SColour::None)*/ { }
	
	CoinStack* CoinStack::GetCoinStack(Colour::Type type)
	{
		ComponentArray<CoinStack>& coin_stacks =  GetAllComponents(CoinStack);
		for( auto iter = coin_stacks.entityToComponent.begin(); iter != coin_stacks.entityToComponent.end(); iter++ )
		{
			CoinStack& coin_stack = coin_stacks.GetComponentByIndex(iter->second);
			if(coin_stack.colourType == type )
			{
				return &coin_stack;
			}
		}

		return nullptr;
	}

	Colour::Type Colour::SColourToType(SColour colour)
	{
		// convert SColour into CoinStack colour
		SColour::Enum colour_type = colour.GetColosestColour();
		switch( colour_type )
		{
			case SColour::White:
				return Colour::White;
			case SColour::Blue:
				return Colour::Blue;
			case SColour::Black:
				return Colour::Black;
			case SColour::Red:
				return Colour::Red;
			case SColour::Green:
				return Colour::Green;
			case SColour::Count:	
			case SColour::None:
			case SColour::Purple:
			case SColour::Yellow:
			case SColour::LightGrey:
			case SColour::MidGrey:
			default:
			break;
		}

		return Colour::Type::Count;
	}


	// Turn
	// ------------------------------------------------------------------
	TurnState::TurnState() : turnIndex(-1), collectedCardSource(EntityInvalid),  canEndTurn(false), initiative(0) { }

	void TurnState::ResetState()
	{
		memset(collectedCoins, 0, sizeof(int) * (int)Colour::Count);
		collectedCardSource = EntityInvalid;
		canEndTurn = false;
	}
	
	bool TurnState::CanAquireMoreResources() const
	{
		// already collect 2 of the same coins
		for( u32 i = 0; i < Colour::Count; i++ )
		{
			if(collectedCoins[i] >= 2)
				return false;
		}

		// already collected 3 different coins
		int coins_collected = 0;
		for( u32 i = 0; i < Colour::Count; i++ )
		{
			coins_collected += collectedCoins[i];
		}
		if(coins_collected >= 3)
			return false;

		// already collect a card
		if(collectedCardSource != EntityInvalid)
			return false;

		return true;
	}

	bool TurnState::HasAquiredResources() const
	{
		// already collect 2 of the same coins
		for( u32 i = 0; i < Colour::Count; i++ )
		{
			if(collectedCoins[i] > 0)
				return true;
		}

		// already collect a card
		if(collectedCardSource != EntityInvalid)
			return true;

		return false;
	}

	// ActionRequest
	// ------------------------------------------------------------------
	ActionRequest::ActionRequest() : request(None), target(EntityInvalid) { }
}