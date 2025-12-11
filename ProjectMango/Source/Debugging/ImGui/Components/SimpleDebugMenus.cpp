#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/Components.h"

u32 DebugMenu::DoHealthDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::Health;

	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::Health& health = GetComponentRef(Health, entity);
		ImGui::PushID(entity + (int)type);

		ImGui::Text("Current Health: %.f", health.currentHealth);
		ImGui::Text("Max Health: %.f", health.maxHealth);

		ImGui::PopID();
	}

	return (u32)type;
}

u32 DebugMenu::DoEntityDataDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::EntityData;

	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::EntityData& entity_data = GetComponentRef(EntityData, entity);
		ImGui::PushID(entity + (int)type);

		ECS::EntityManager& em = ecs->entities;
		const char* parent = entity_data.parent != ECS::EntityInvalid ? ECS::GetName(entity_data.parent) : "No parent";
		ImGui::Text("Parent: %s", parent);

		for( u32 i = 0; i < entity_data.children.size(); i++ )
		{
			const char* child = ECS::GetName(entity_data.children[i]);
			ImGui::Text("Child: %s", child);
		}

		ImGui::PopID();
	}

	return (u32)type;
}

u32 DebugMenu::DoCoinStackDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::CoinStack;

	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::CoinStack& coin_stack = GetComponentRef(CoinStack, entity);
		ImGui::PushID(entity + (int)type);
		
		SColour col = coin_stack.colour;
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(col.r, col.g, col.b, 255));
		ImGui::Text("Type: %d", coin_stack.coinType);
		ImGui::PopStyleColor();

		ImGui::Text("Remaining: %d", coin_stack.remaining);
		ImGui::Text("Capacity: %d", coin_stack.capacity);

		if(ImGui::Button("Restock"))
		{
			coin_stack.remaining = coin_stack.capacity;
		}

		ImGui::PopID();
	}

	return (u32)type;
}

u32 DebugMenu::DoInventoryDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::Inventory;

	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::Inventory& inventory = GetComponentRef(Inventory, entity);
		ImGui::PushID(entity + (int)type);
		
		ImGui::Text("Owned coins");
		for( u32 i = 0; i < ECS::Coin::Count; i++ )
		{
			ImGui::Text("%d: %d", i, inventory.coins[i]);
		}

		ImGui::PopID();
	}

	return (u32)type;
}

u32 DebugMenu::DoCardDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::Card;

	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::Card& card = GetComponentRef(Card, entity);
		ImGui::PushID(entity + (int)type);
		
		ImGui::Text("cost");
		for( u32 i = 0; i < ECS::Coin::Count; i++ )
		{
			SColour colour = ECS::Coin::s_typeToColour.at((ECS::Coin::Type)i);
			if(colour.type == SColour::Black)
				colour = SColour(SColour::MidGrey);
			
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(colour.r, colour.g, colour.b, 255));
			ImGui::Text("%d, ", card.cost[i]);
			ImGui::PopStyleColor();
			ImGui::SameLine();
		}

		ImGui::PopID();
	}

	return (u32)type;
}