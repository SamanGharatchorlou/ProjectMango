#pragma once

// all the more game speicifc components in here
namespace ECS
{
	struct Colour
	{
		enum Type
		{
			White,
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
	};

	struct CoinStack
	{
		COMPONENT_TYPE(CoinStack)

		Colour::Type colourType;
		//SColour colour;

		int remaining;
		int capacity;

		static CoinStack* GetCoinStack(Colour::Type type);
	};

	struct Card
	{
		static constexpr int c_tiers = 3;

		COMPONENT_TYPE(Card)

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
	};

	struct Inventory
	{
		COMPONENT_TYPE(Inventory)

		// amount of coins owned owns
		int coins[Colour::Count] { 0 };

		// cards we own
		std::vector<int> cards;

		void GetCardPower(int array[], int size) const;
		void GetBuyingPower(int array[], int size) const; 
	};

	struct TurnState
	{
		COMPONENT_TYPE(TurnState)

		int turnIndex;
		int initiative;
		bool canEndTurn;

		int collectedCoins[Colour::Count] { 0 };

		Card collectedCard;
		Entity collectedCardSource;

		void ResetState();
		bool CanAquireMoreResources() const;
		bool HasAquiredResources() const;
	};

	struct ActionRequest
	{
		COMPONENT_TYPE(ActionRequest)

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