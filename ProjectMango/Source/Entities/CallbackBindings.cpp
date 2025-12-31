#include "pch.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"

using namespace ECS;

static void UpdateCostIcons(Entity entity)
{
	CoinStack& coin_stack = GetComponentRef(CoinStack, entity);
	std::vector<Entity>& costs = coin_stack.costEntities;

	int visible_cost = 0;
	for( u32 i = 0; i < costs.size(); i++ )
	{
		const Sprite& sprite = GetComponentRef(Sprite, costs[i]);
		if(!sprite.params.disabled)
			visible_cost++;
	}
	 
	int diff = coin_stack.remaining - visible_cost;

	// there are more coins than we are showing
	if(diff > 0)
	{
		for( int i = visible_cost; i < coin_stack.remaining; i++ )
		{
			Sprite& sprite = GetComponentRef(Sprite, costs[i]);
			sprite.params.disabled = false;
		}
	}
	// we are showing more coins than there are
	else if(diff < 0)
	{
		for( int i = (visible_cost - 1); i >= coin_stack.remaining; i-- )
		{
			Sprite& sprite = GetComponentRef(Sprite, costs[i]);
			sprite.params.disabled = true;
		}
	}
}

static void UpdateCoinPile(Entity coin_pile, Entity owner)
{
	if(Inventory* inventory = GetComponent(Inventory, owner))
	{
		CoinStack& coin_stack = GetComponentRef(CoinStack, coin_pile);
		coin_stack.remaining = inventory->coins[coin_stack.colourType];

		UpdateCostIcons(coin_pile);
	}
}

static void UpdateCardPower(Entity coin_pile, Entity owner)
{
	if(Inventory* inventory = GetComponent(Inventory, owner))
	{
		CoinStack& coin_stack = GetComponentRef(CoinStack, coin_pile);

		int card_power[Colour::Count];
		inventory->GetCardPower(card_power, Colour::Count);

		coin_stack.remaining = card_power[coin_stack.colourType];

		UpdateCostIcons(coin_pile);
	}
}

static void UpdateLayeredHealthBar(const Health* health, Entity health_bar_ui)
{
	if(LayeredSprite* layered_sprite = GetComponent(LayeredSprite, health_bar_ui))
	{
		LayeredSprite::Layer& layer = layered_sprite->spriteLayers[1];

		float remaining_health = health->currentHealth / health->maxHealth;
		
		const Transform& transform = GetComponentRef(Transform, health_bar_ui);
		VectorF t_size = transform.size;
		VectorF pos = transform.worldPosition + VectorF(0.0f, t_size.y * (1-remaining_health));
		VectorF size = VectorF(t_size.x, t_size.y * remaining_health);
		layer.rect = RectF(pos, size);
	}
}

void SetupCallbackBindings(std::unordered_map<BasicString, std::function<void(ECS::Entity)>>& callback_bindings)
{
	callback_bindings[ "PlayerHealthBar" ] =  [](ECS::Entity entity) {
		if(const Health* health = GetComponent(Health, Faction::GetPlayer()))
			UpdateLayeredHealthBar(health, entity);
	};

	callback_bindings[ "AIHealthBar" ] =  [](ECS::Entity entity) {
		if(const Health* health = GetComponent(Health, Faction::GetEnemy()))
			UpdateLayeredHealthBar(health, entity);
	};

	callback_bindings[ "InventoryCoinPile" ] =  [](ECS::Entity entity) {
		UpdateCoinPile(entity, Faction::GetPlayer());
	};

	callback_bindings[ "InventoryCardPower" ] =  [](ECS::Entity entity) {
		UpdateCardPower(entity, Faction::GetPlayer());
	};

	callback_bindings[ "AICardPower" ] =  [](ECS::Entity entity) {
		UpdateCardPower(entity, Faction::GetEnemy());
	};

	callback_bindings[ "EnemyCoinPile" ] =  [](ECS::Entity entity) {
		UpdateCoinPile(entity, Faction::GetEnemy());
	};

	callback_bindings[ "CoinStack" ] =  [](ECS::Entity entity) {
		const CoinStack& coin_stack = GetComponentRef(CoinStack, entity);
		SpriteSheet& sprite_sheet = GetComponentRef(SpriteSheet, entity);
		sprite_sheet.index = coin_stack.remaining-1;
	};
}