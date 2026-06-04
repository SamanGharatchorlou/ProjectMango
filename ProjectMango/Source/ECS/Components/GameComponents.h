#pragma once

// when adding a component, define it in EntityCommon.h
// then setup how its updated in ComponentsSetup

// all the more game speicifc components in here
namespace ECS
{
	struct EntityMetaData;
	 
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
			{ "white",	White }, 
			{ "blue",	Blue }, 
			{ "black",	Black },
			{ "red",	Red },
			{ "green",	Green } 
		};

		inline static const std::unordered_map<Type, StringBuffer32> s_typeToString { 
			{ White,	"white" }, 
			{ Blue,		"blue" }, 
			{ Black,	"black" },
			{ Red,		"red" },
			{ Green,	"green" } 
		};
				
		inline static const std::unordered_map<Type, SColour> s_typeToColour { 
			{ White,	SColour::CardWhite }, 
			{ Blue,		SColour::CardBlue }, 
			{ Black,	SColour::CardBlack },
			{ Red,		SColour::CardRed },
			{ Green,	SColour::CardGreen } 
		};

		static constexpr u32 s_defaultNoColour = 0xF1F1F1;

		static Type SColourToType(SColour colour);

		COMPONENT_TYPE(Colour)

		// probably duplicate data in some cases, like coinstack already has it, but i also need this
		Colour::Type colour = Count;
	};
	
	void AddColourPostfix(const char* postfix, Colour::Type colour, StringBuffer64& out_string);

	struct CoinStack
	{
		COMPONENT_TYPE(CoinStack)

		static constexpr const char* kRequirement = "coin_capacity";

		Colour::Type colourType = Colour::Count;

		int capacity = -1;
		int remaining = -1;

		Entity owner = EntityInvalid;

		std::vector<Entity> costEntities;

		void Init(const EntityMetaData& emd);
	};

	struct Card
	{
		static constexpr int c_tiers = 3;

		COMPONENT_TYPE(Card)

		static constexpr const char* kRequirement = "card_registry_index";
		void Init(const EntityMetaData& emd);

		Colour::Type colour;

		// what to pay to aquire the card
		int cost[Colour::Count] { 0 };
		int discount[Colour::Count] { 0 };

		std::vector<Entity> costEntities[Colour::Count];

		// how many coins it provides once owned
		// turn into a simple colour, doesnt need to be an array
		int power[Colour::Count] { 0 };

		// points... for something, not sure yet
		int points = 0;

		// tier 1,2,3
		int tier = 0;

		int registryIndex = 0;
		VectorI boardIndex;

		BasicString spell;

		void BuildChildDisplays();
		bool CanAfford(Entity entity) const;

		int Cost(u32 index) const;
		bool IsColour(Colour::Type colour) const;
	};


	struct DeckCard
	{
		int tier;
		int registryIndex;
		int weight = 1;

		bool operator == (const DeckCard& dc) const { return tier == dc.tier && registryIndex == dc.registryIndex; }
	};

	struct CardBoard
	{
		COMPONENT_TYPE(CardBoard)

		static constexpr const char* kRequirement = "card_board";
		void Init(const EntityMetaData& emd) {}

		Grid<ECS::Entity> cards;

		std::vector<DeckCard>	drawPile[Card::c_tiers];
		std::vector<int>		discardPile[Card::c_tiers];

		std::vector<VectorI> triggeredCards;
	};

	enum class GameEvent
	{
		None,
		DrawPileBuilt,
		CardDrawn,
	};


	// turn into a component? does it need to be, dont think so
	struct Relic
	{
		typedef void(*EffectFn)(const Relic& relic, Entity entity);

		enum Phase
		{
			Selection,	// run first, card selection so we want other effects after this
			Effect,		// default
			Count
		};

		BasicString id;
		BasicString description;
		GameEvent trigger;
		EffectFn effectFn;
		Phase phase = Effect;

		// might only affect a specific colour
		Colour::Type colour = Colour::Count;

		bool operator == (const Relic& relic) const { return effectFn == relic.effectFn && colour == relic.colour; }
	};

	struct StatusEffect
	{
		BasicString type;
		
		int turnDuration = 0; 
		int turnApplied = 0; 

		bool onApplied = false;

		// might only affect a specific colour
		Colour::Type colour = Colour::Count;

		static void Create(const char* effect, StatusEffect& out_effect);

		bool operator==(const StatusEffect& rhs) const = default;
		//bool operator == (const StatusEffect& rhs)
		//{
		//	return type == rhs.type && 
		//			turnDuration == rhs.turnDuration &&
		//			turnApplied == rhs.turnApplied;
		//}
	};

	struct StatusEffects
	{
		COMPONENT_TYPE(StatusEffects)

		std::vector<StatusEffect> effects;
	};

	void ApplyStatusEffect(const char* effect, Entity target);

	struct Inventory
	{
		COMPONENT_TYPE(Inventory)
		Inventory();

		// amount of coins owned owns
		int coins[Colour::Count] { 0 };

		// cards we own
		std::vector<int> cards;

		// relics we own
		std::vector<Relic> relics;
		// disabled relics - these dont accept events
		std::vector<BasicString> disabledRelicIds;

		// array size always Colour::Count
		void GetCardPower(int array[]) const;
		void GetBuyingPower(int array[]) const;

		int GetPoints() const;
	};

	void TriggerGameEvent(GameEvent event, Entity entity);

	struct TurnState
	{
		COMPONENT_TYPE(TurnState)

		int turnIndex = 0;
		int initiative = 0;
		
		bool tryEndTurn = false;
		bool canEndTurn = false;

		// a cooldown time between ending the turn and moving onto the next turn state
		float endTurnCooldownSecs = 0;

		// player specific state
		int collectedCoins[Colour::Count] { 0 };
		int collectedCardRegistryIndex = -1;

		void ResetState();
		bool CanAquireMoreResources() const;
		bool HasAquiredResources() const;
		bool CanCollectCoin(Colour::Type colour) const;

		static TurnState* GetActive();
		static bool IsCurrentTurn(Entity entity);

		bool IsCurrentTurn() const;


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
			ReturnCoins
		};

		Type request;

		// coin stack to collect from, card to aquire
		Entity target;
	};


}