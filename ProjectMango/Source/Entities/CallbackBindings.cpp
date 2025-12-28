#include "pch.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"

using namespace ECS;

static void UpdateCoinPile(Entity coin_pile, Entity owner)
{
	if(Inventory* inventory = GetComponent(Inventory, owner))
	{
		CoinStack& coin_stack = GetComponentRef(CoinStack, coin_pile);
		coin_stack.remaining = inventory->coins[coin_stack.colourType];

		SpriteSheet& sprite_sheet = GetComponentRef(SpriteSheet, coin_pile);
		sprite_sheet.index = coin_stack.remaining;
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
		if(const Health* health = GetComponent(Health, Target::GetPlayer()))
			UpdateLayeredHealthBar(health, entity);
	};

	callback_bindings[ "AIHealthBar" ] =  [](ECS::Entity entity) {
		if(const Health* health = GetComponent(Health, Target::GetEnemy()))
			UpdateLayeredHealthBar(health, entity);
	};

	callback_bindings[ "InventoryCoinPile" ] =  [](ECS::Entity entity) {
		UpdateCoinPile(entity, Target::GetPlayer());
	};

	callback_bindings[ "EnemyCoinPile" ] =  [](ECS::Entity entity) {
		UpdateCoinPile(entity, Target::GetEnemy());
	};

	callback_bindings[ "CoinStack" ] =  [](ECS::Entity entity) {
		const CoinStack& coin_stack = GetComponentRef(CoinStack, entity);
		SpriteSheet& sprite_sheet = GetComponentRef(SpriteSheet, entity);
		sprite_sheet.index = coin_stack.remaining-1;
	};
}