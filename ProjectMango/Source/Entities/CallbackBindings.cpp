#include "pch.h"

#include "ECS/Components/Components.h"
#include "ECS/Components/GameComponents.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/AIComponents.h"

using namespace ECS;

static void UpdateCoinPile(Entity coin_pile, Entity owner)
{
	if(Inventory* inventory = GetComponent(Inventory, owner))
	{
		CoinStack& coin_stack = GetComponentRef(CoinStack, coin_pile);
		coin_stack.remaining = inventory->coins[coin_stack.colourType];

		//int buying_power[Colour::Count];
		//inventory->GetBuyingPower(buying_power, Colour::Count);

		//int sprite_index = coin_stack.remaining;

		//Sprite& sprite = GetComponentRef(Sprite, coin_pile);
		SpriteSheet& sprite_sheet = GetComponentRef(SpriteSheet, coin_pile);
		sprite_sheet.index = coin_stack.remaining;


		//char buffer[32];
		//snprintf(buffer, 32, "%s_%d", sprite.Id.c_str(), sprite_index);
		//sprite.SetTexture(buffer);
	}
}

void SetupCallbackBindings(std::unordered_map<BasicString, std::function<void(ECS::Entity)>>& callback_bindings)
{
	callback_bindings[ "PlayerHealthBar" ] =  [](ECS::Entity entity) {
		if(const Health* health = GetComponent(Health, Target::GetPlayer()))
		{
			if(LayeredSprite* layered_sprite = GetComponent(LayeredSprite, entity))
			{
				LayeredSprite::Layer& layer = layered_sprite->spriteLayers[1];
				const Transform& transform = GetComponentRef(Transform, entity);

				float remaining_health = health->currentHealth / health->maxHealth;

				VectorF t_size = transform.size;

				VectorF pos = transform.worldPosition + VectorF(0.0f, t_size.y * (1-remaining_health));
				VectorF size = VectorF(t_size.x, t_size.y * remaining_health);
				layer.rect = RectF(pos, size);
			}
		} };

	callback_bindings[ "AIHealthBar" ] =  [](ECS::Entity entity) {
		Entity ai = Target::GetEnemy();
		if(const Health* health = GetComponent(Health, ai))
		{
			if(LayeredSprite* layered_sprite = GetComponent(LayeredSprite, entity))
			{
				LayeredSprite::Layer& layer = layered_sprite->spriteLayers[1];
				const Transform& transform = GetComponentRef(Transform, entity);

				float remaining_health = health->currentHealth / health->maxHealth;

				VectorF t_size = transform.size;

				VectorF pos = transform.worldPosition + VectorF(0.0f, t_size.y * (1-remaining_health));
				VectorF size = VectorF(t_size.x, t_size.y * remaining_health);
				layer.rect = RectF(pos, size);
			}
		} };

	callback_bindings[ "InventoryCoinPile" ] =  [](ECS::Entity entity) {
		UpdateCoinPile(entity, Target::GetPlayer());
	};

	callback_bindings[ "CoinStack" ] =  [](ECS::Entity entity) {
		CoinStack& coin_stack = GetComponentRef(CoinStack, entity);
		//int remaining = coin_stack.remaining;

		//Sprite& sprite = GetComponentRef(Sprite, entity);

		SpriteSheet& sprite_sheet = GetComponentRef(SpriteSheet, entity);
		sprite_sheet.index = coin_stack.remaining-1;

		//char buffer[32];
		//snprintf(buffer, 32, "%s_%d", sprite.Id.c_str(), remaining);
		//sprite.SetTexture(buffer);
	};
		
	callback_bindings[ "EnemyCoinPile" ] =  [](ECS::Entity entity) {
		Entity ai = Target::GetEnemy();
		//UpdateCoinPile(entity, ai);
	};
}