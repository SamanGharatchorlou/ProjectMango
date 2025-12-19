#pragma once

// all the more game speicifc components in here
namespace ECS
{
	struct Colour
	{
		enum Type
		{
			White, // #d1d1d1
			Blue,
			Black,
			Red,
			Green,
			Count
		};

		inline static const std::unordered_map<StringBuffer32, Type> s_stringToType { 
			{ "White",	White }, 
			{ "Blue",	Blue }, 
			{ "Black",	Black },
			{ "Red",	Red },
			{ "Green",	Green } 
		};

		inline static const std::unordered_map<Type, StringBuffer32> s_typeToString { 
			{ White,	"White" }, 
			{ Blue,		"Blue" }, 
			{ Black,	"Black" },
			{ Red,		"Red" },
			{ Green,	"Green" } 
		};
				
		inline static const std::unordered_map<Type, SColour> s_typeToColour { 
			{ White,	SColour::White }, 
			{ Blue,		SColour::Blue }, 
			{ Black,	SColour::Black },
			{ Red,		SColour::Red },
			{ Green,	SColour::Green } 
		};

		static constexpr u32 s_defaultNoColour = 0xF1F1F1;

		static Type SColourToType(SColour colour);

		COMPONENT_TYPE(Colour)

		// probably duplicate data in some cases, like coinstack already has it, but i also need this
		Colour::Type colour;
	};

	struct CoinStack
	{
		COMPONENT_TYPE(CoinStack)

		Colour::Type colourType = Colour::Count;
		
		// sprite = spritePrefix + remaining
		// e.g. spritePrefix = "BlueCoin" then we have sprite = "BlueCoin2"
		BasicString spritePrefix;

		int capacity = -1;
		int remaining = -1;
		
		// special case, these are the inventory coin piles, there's probably a
		// better way to do this, but i dont want to make a new component just for this
		bool isInventory = false;

		static CoinStack* GetCoinStack(Colour::Type type);
	};

	struct Card
	{
		static constexpr int c_tiers = 3;

		COMPONENT_TYPE(Card)
		Card();

		Colour::Type colour;

		// what to pay to aquire the card
		int cost[Colour::Count] { 0 };

		// how many coins it provides once owned
		int power[Colour::Count] { 0 };

		// points... for something, not sure yet
		int points;

		// tier 1,2,3
		int tier;

		int cardRegistryIndex;

		void RegenerateChildDisplays();
		bool CanAfford(Entity entity) const;
	};

	struct Inventory
	{
		COMPONENT_TYPE(Inventory)
		Inventory();

		// amount of coins owned owns
		int coins[Colour::Count] { 0 };

		// cards we own
		std::vector<int> cards;

		void GetCardPower(int array[], int size) const;
		void GetBuyingPower(int array[], int size) const;

		int GetPoints() const;
	};

	struct TurnState
	{
		COMPONENT_TYPE(TurnState)
		TurnState();

		int turnIndex;
		int initiative;
		bool canEndTurn;
		bool isActiveTurn;

		Entity attackingMonster;

		int collectedCoins[Colour::Count] { 0 };

		int collectedCardRegIndex;
		Entity collectedCardSource;

		void ResetState();
		bool CanAquireMoreResources() const;
		bool HasAquiredResources() const;
	};

	struct ActionRequest
	{
		COMPONENT_TYPE(ActionRequest)
		ActionRequest();

		enum Type
		{
			None,
			CollectCoin,
			AquireCard,
			EndTurn,
			UndoTurn
		};

		Type request;

		// coin stack to collect from, card to aquire
		Entity target;
	};
}