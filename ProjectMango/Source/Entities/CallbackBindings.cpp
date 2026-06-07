#include "pch.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Core/Helpers.h"
#include "Entities/Registries/ResourceBank.h"
#include "Entities/Registries/RelicRegistry.h"

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
		// BUG: visible_cost = 5 and coin_stack.remaining = 6, that should never be 6 need to clamp it somewhere
		// getting cards via debug though
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
		coin_stack.remaining = Maths::clamp(coin_stack.remaining, 0, coin_stack.capacity);

		UpdateCostIcons(coin_pile);
	}
}

static void UpdateCardPower(Entity coin_pile, Entity owner)
{
	if(Inventory* inventory = GetComponent(Inventory, owner))
	{
		CoinStack& coin_stack = GetComponentRef(CoinStack, coin_pile);

		int card_power[Colour::Count];
		inventory->GetCardPower(card_power);

		coin_stack.remaining = card_power[coin_stack.colourType];
		coin_stack.remaining = Maths::clamp(coin_stack.remaining, 0, coin_stack.capacity);

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

void SetupCallbackUpdates(std::unordered_map<BasicString, std::function<void(ECS::Entity)>>& callback_updates)
{
	callback_updates[ "PlayerHealthBar2" ] =  [](ECS::Entity entity) {
		if(const Health* health = GetComponent(Health, Faction::GetPlayer()))
			UpdateLayeredHealthBar(health, entity);
	};

	callback_updates["PlayerHealthBar"] = [](ECS::Entity entity) {
		if (const Health* health = GetComponent(Health, Faction::GetPlayer()))
		{
			float percentage = health->currentHealth / health->maxHealth;
			VectorF size = GetRect(entity).Size();

			Sprite* sprite = GetComponent(Sprite, entity);
			sprite->params.renderOffset = VectorF(size.x * (1.0f - percentage), 0.0f);
			sprite->params.clipping = true;
		}
	};
	callback_updates["EnemyHealthBar"] = [](ECS::Entity entity) {
		if (const Health* health = GetComponent(Health, Faction::GetEnemy()))
		{
			float percentage = health->currentHealth / health->maxHealth;
			VectorF size = GetRect(entity).Size();

			Sprite* sprite = GetComponent(Sprite, entity);
			sprite->params.renderOffset = VectorF(size.x * (1.0f - percentage), 0.0f);
			sprite->params.clipping = true;
		}
	};


	callback_updates[ "AIHealthBar" ] =  [](ECS::Entity entity) {
		if(const Health* health = GetComponent(Health, Faction::GetEnemy()))
			UpdateLayeredHealthBar(health, entity);
	};

	callback_updates[ "InventoryCoinPile" ] =  [](ECS::Entity entity) {
		UpdateCoinPile(entity, Faction::GetPlayer());
	};

	callback_updates[ "InventoryCardPower" ] =  [](ECS::Entity entity) {
		UpdateCardPower(entity, Faction::GetPlayer());
	};

	callback_updates[ "AICardPower" ] =  [](ECS::Entity entity) {
		UpdateCardPower(entity, Faction::GetEnemy());
	};

	callback_updates[ "EnemyCoinPile" ] =  [](ECS::Entity entity) {
		UpdateCoinPile(entity, Faction::GetEnemy());
	};

	callback_updates[ "CoinStack" ] =  [](ECS::Entity entity) {
		const CoinStack& coin_stack = GetComponentRef(CoinStack, entity);
		SpriteSheet& sprite_sheet = GetComponentRef(SpriteSheet, entity);
		sprite_sheet.index = coin_stack.remaining-1;
	};
}

// todo: move this somewhere else?
static void SetupCostIcons(Entity entity, int count)
{
	CoinStack& coin_stack = GetComponentRef(CoinStack, entity);

	DestroyChildren(entity);
	coin_stack.costEntities.resize(count);

	const Transform& transform = GetComponentRef(Transform, entity);
	VectorF size = transform.size;
	VectorF bottom = VectorF(size.x * 0.1f, size.y * 0.85f);

	for (u32 i = 0; i < count; i++)
	{
		Entity child_entity = CreateEntity("coin_icon");
		coin_stack.costEntities[i] = (child_entity);

		EntityData::SetParent(child_entity, entity);

		// Transform
		VectorF size = VectorF(15, 15);
		size = AdjustToScreenSize(size);
		Transform& child_transform = AddComponent(Transform, child_entity);
		child_transform.size = size;

		VectorF offset = VectorF(0.0f, i * 1.25f);
		VectorF local_position = bottom - (child_transform.size * offset);
		child_transform.SetLocalPosition(local_position);

		// Sprite
		Sprite& child_sprite = AddComponent(Sprite, child_entity);
		child_sprite.SetTexture("cost_filled");
		child_sprite.params.renderLayer = RenderLayer::UI;
		child_sprite.params.colourMod = Colour::s_typeToColour.at(coin_stack.colourType);
		child_sprite.params.colourMod.setOpacity(0.6f);
	}
}

static void SetupPowerIcons(Entity entity, int count)
{
	CoinStack& coin_stack = GetComponentRef(CoinStack, entity);

	DestroyChildren(entity);
	coin_stack.costEntities.resize(count);

	const Transform& transform = GetComponentRef(Transform, entity);
	VectorF size = transform.size;
	VectorF top = VectorF(size.x * 0.1f, size.y * 0.1f);

	for (u32 i = 0; i < count; i++)
	{
		Entity child_entity = CreateEntity("coin_icon");
		coin_stack.costEntities[i] = (child_entity);

		EntityData::SetParent(child_entity, entity);

		// Transform
		VectorF size = VectorF(15, 15);
		size = AdjustToScreenSize(size);

		Transform& child_transform = AddComponent(Transform, child_entity);
		child_transform.size = size;

		VectorF offset = VectorF(0.0f, i * 1.25f);
		VectorF local_position = top + (child_transform.size * offset);
		child_transform.SetLocalPosition(local_position);

		// Sprite
		Sprite& child_sprite = AddComponent(Sprite, child_entity);
		child_sprite.SetTexture("power_filled");
		child_sprite.params.renderLayer = RenderLayer::UI;
		child_sprite.params.colourMod = Colour::s_typeToColour.at(coin_stack.colourType);
		child_sprite.params.colourMod.setOpacity(0.6f);
	}
}


void SetupCallbackInits(std::unordered_map<BasicString, std::function<void(ECS::Entity)>>& callback_inits)
{
	callback_inits["CoinStack"] = [](ECS::Entity entity) {
		CoinStack& coin_stack = GetComponentRef(CoinStack, entity);
		coin_stack.remaining = coin_stack.capacity;

		RegisterCoinResource(entity);
	};

	callback_inits["InventoryCardPower"] = [](ECS::Entity entity) {
		CoinStack& coin_stack = GetComponentRef(CoinStack, entity);
		SetupPowerIcons(entity, coin_stack.capacity);
	};

	callback_inits["InventoryCoinPile"] = [](ECS::Entity entity) {
		CoinStack& coin_stack = GetComponentRef(CoinStack, entity);
		SetupCostIcons(entity, coin_stack.capacity);
	};

	callback_inits["RandomRelicChoicePanel"] = [](ECS::Entity entity) {
		Inventory& inventory = AddComponent(Inventory, entity);
		if(ECS::Relic* relic = RelicRegistry::GetRandomUnobtainedRelic())
		{
			inventory.relics.push_back(*relic);

			if (SpriteSheet* ss = GetComponent(SpriteSheet, entity))
			{
				ss->index = relic->colour;
			}
		}
	};
	
}