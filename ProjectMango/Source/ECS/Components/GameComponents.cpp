#include "pch.h"
#include "GameComponents.h"

#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "ECS/EntityCommon.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/IncludeComponents.h"
#include "Entities/CardRegistry.h"
#include "Entities/MonsterRegistry.h"
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

	int Inventory::GetPoints() const
	{
		int total_points = 0;
		for( int card_index : cards )
		{
			const Card* card = CardRegistry::LookupCard(card_index);
			total_points += card->points;
		}

		return total_points;
	}

	// Card
	// ------------------------------------------------------------------
	bool Card::CanAfford(Entity entity) const
	{
		if(Inventory* inventory = GetComponent(Inventory, entity))
		{
			if(DebugMenu::GetState().canBuyAnyCard)
			{
				if(Target::GetPlayer() == entity)
					return true;
			}

			int buying_power[Colour::Count];
			inventory->GetBuyingPower(buying_power, Colour::Count);
			for( u32 i = 0; i < Colour::Count; i++ )
			{
				if(buying_power[i] < cost[i])
					return false;
			}

			return true;
		}

		return false;
	}
	
	Entity Card::GetMonster() const
	{
		std::vector<Entity> children;
		GetChildren(entity, children);
		for( u32 i = 0; i < children.size(); i++ )
		{
			// a bit random to get the animator, but it works...
			if(HasComponent(Animator, children[i]))
			{
				return children[i];
			}
		}

		return EntityInvalid;
	}

	static void GenerateColouredGems(Entity entity)
	{
		Card& card = GetComponentRef(Card, entity);		
		const Transform& transform = GetComponentRef(Transform, entity);
		const Sprite& sprite = GetComponentRef(Sprite, entity);

		int count = 0;
		for( u32 i = 0; i < Colour::Count; i++ )
		{
			if(card.cost[i] > 0)
			{
				VectorF gem_size = VectorF(32,32);
				Entity child_entity = CreateBasicObject("card_icon", gem_size);
				EntityData::SetParent(child_entity, entity);

				Transform& child_transform = GetComponentRef(Transform, child_entity);
				// where we want to set the text bounds
				VectorF visible_size = child_transform.size * 0.8f;
				VectorF position = (transform.size - child_transform.size) * VectorF(0.1f, 0.9f);
				
				// use a % of the height so they sit tighter together
				float effective_height = child_transform.size.y * 1.0f;
				position += VectorF( 0, effective_height * -(float)count ) ;

				child_transform.SetLocalPosition( position );
			
				char buffer[32];
				const StringBuffer32& colour_string = Colour::s_typeToString.at((Colour::Type)i);
				snprintf(buffer, 32, "gem_%s", colour_string.c_str());

				Sprite& child_sprite = GetComponentRef(Sprite, child_entity);
				child_sprite.SetTexture(buffer);
				child_sprite.params.renderLayer = (RenderLayer)((int)sprite.params.renderLayer + 1);

				BasicString text(card.cost[i]);

				UIText& child_text_display = AddComponent(UIText, child_entity);
				child_text_display.SetText( text.c_str() );
				child_text_display.FitToSize(visible_size);
				child_text_display.SetRenderOffsetToCenter();
				child_text_display.renderOffset += VectorF(0,2);
				if((Colour::Type)i == Colour::White)
					child_text_display.SetColour(SColour::Black);
				else
					child_text_display.SetColour(SColour::White);

				count++;
			}
		}
	}

	static void GenerateCostIcons(Entity entity)
	{
		Card& card = GetComponentRef(Card, entity);		
		const Transform& transform = GetComponentRef(Transform, entity);
		const Sprite& sprite = GetComponentRef(Sprite, entity);

		VectorF size = transform.size;
		VectorF bottom = VectorF(size.x * 0.1f, size.y * 0.85f);

		int count = 0;
		for( int i = 0; i < Colour::Count; i++ )
		{
			if(card.cost[i] == 0)
				continue;

			Colour::Type colour = (Colour::Type)i;

			for( int j = 0; j < card.cost[i]; j++ )
			{
				Entity child_entity = CreateEntity("card_icon");
				card.costEntities[i].push_back(child_entity);

				EntityData::SetParent(child_entity, entity);

				// Transform
				VectorF size = VectorF(9,9);
				Transform& child_transform = AddComponent(Transform, child_entity);
				child_transform.size = size;

				VectorF offset =  VectorF(-count * 1.5f, j * 1.25f);
				VectorF local_position = bottom - (child_transform.size * offset);
				child_transform.SetLocalPosition( local_position );

				// Sprite
				Sprite& child_sprite = AddComponent(Sprite, child_entity);
				child_sprite.SetTexture("cost_empty");
				child_sprite.params.renderLayer = RenderLayer::UI;
				child_sprite.params.colourMod = Colour::s_typeToColour.at(colour);
				child_sprite.params.colourMod.setOpacity(0.7f);

			}

			count++;
		}
	}

	void Card::RegenerateChildDisplays()
	{
		DestroyChildren(entity);
		for( int i = 0; i < Colour::Count; i++ )
		{
			costEntities->resize(0);
		}

		const Transform& transform = GetComponentRef(Transform, entity);
		const Sprite& sprite = GetComponentRef(Sprite, entity);
		
		VectorF child_size = VectorF(transform.size.x, transform.size.x) * 0.5f;

		if(points > 0)
		{
			// build points
			Entity points_entity = CreateEntity("card_points");
			EntityData::SetParent(points_entity, entity);

			// Transform
			Transform& points_transform = AddComponent(Transform, points_entity);
			points_transform.size = child_size * 0.5f;
			points_transform.SetLocalPosition( transform.size * (VectorF(0.025f, 0.03f) ) );
			
			// UIText
			UIText& ui_text = AddComponent(UIText, points_entity);
			ui_text.center = true;
			ui_text.SetColour(SColour::White);
			ui_text.SetSize(27);

			BasicString number_to_text = BasicString(points);
			ui_text.SetText(number_to_text.c_str());
		}

		GenerateCostIcons(entity );

		if(monsterRegistryIndex != -1)
		{
			const char* monster = MonsterRegistry::GetMonster(monsterRegistryIndex);
			CreateCardActor(monster, entity);
		}
	}


	// CoinStack
	// ------------------------------------------------------------------
	CoinStack* CoinStack::GetCoinStack(Colour::Type type)
	{
		ComponentArray<CoinStack>& coin_stacks =  GetAllComponents(CoinStack);
		for( auto iter = coin_stacks.entityToComponent.begin(); iter != coin_stacks.entityToComponent.end(); iter++ )
		{
			CoinStack& coin_stack = coin_stacks.GetComponentByIndex(iter->second);
			if(coin_stack.isInventory)
				continue;

			if(coin_stack.colourType == type )
			{
				return &coin_stack;
			}
		}

		return nullptr;
	}


	// Turn
	// ------------------------------------------------------------------
	TurnState::TurnState() : 
		isActiveTurn(false),
		turnIndex(0), 
		collectedCardSource(EntityInvalid),  
		collectedCardRegIndex(-1),
		canEndTurn(false), 
		initiative(0),
		attackingMonster(EntityInvalid)
	{ }

	void TurnState::ResetState()
	{
		memset(collectedCoins, 0, sizeof(int) * (int)Colour::Count);
		collectedCardSource = EntityInvalid;
		canEndTurn = false;
		collectedCardRegIndex = -1;
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

		// collected some coins, can it collect anymore (coin stacks might be empty)
		if(coins_collected > 0)
		{
			std::vector<Colour::Type> available_coin_stacks;

			for( u32 i = 0; i < Colour::Count; i++ )
			{
				if(CoinStack::GetCoinStack((Colour::Type)i)->remaining > 0)
				{
					available_coin_stacks.push_back((Colour::Type)i);
				}
			}
			
			// if there's any other available coin stack to pull from that has been already, then we can continue
			bool has_available_coin_stack = false;
			for( Colour::Type available_coin_stack : available_coin_stacks )
			{
				if(collectedCoins[available_coin_stack] == 0)
				{
					has_available_coin_stack = true;
					break;
				}
			}

			if(!has_available_coin_stack)
				return false;
		}
		else if(collectedCardSource != EntityInvalid)
		{
			return false;
		}

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


	// Colour
	// ------------------------------------------------------------------
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
		
	void AddColourPostfix(const char* postfix, Colour::Type colour, StringBuffer64& out_string)
	{
		if(colour != -1)
		{
			const StringBuffer32& colour_string = Colour::s_typeToString.at(colour);
			snprintf(out_string.buffer(), out_string.bufferLength(), "%s_%s", postfix, colour_string.c_str() );
		}
	}

}