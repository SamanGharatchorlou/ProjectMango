#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/GameComponents.h"

u32 DebugMenu::DoHealthDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::Health;

	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::Health& health = GetComponentRef(Health, entity);
		ImGui::PushID(entity + (int)type);

		ImGui::Text("Current: %.f", health.currentHealth);
		ImGui::Text("Max: %.f", health.maxHealth);

		if(ImGui::Button("Reduce by 10"))
		{
			health.currentHealth= health.currentHealth - 10.0f;
		}
				
		if(ImGui::Button("Reset"))
		{
			health.currentHealth = health.maxHealth;
		}

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
		
		SColour col = ECS::Colour::s_typeToColour.at(coin_stack.colourType);
		ECS::Colour::Type c_type = coin_stack.colourType;
		//ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(col.r, col.g, col.b, 255));
		//ImGui::Text("Type: %d", coin_stack.coinType);
		//ImGui::PopStyleColor();

		ImGui::Text("Remaining: %d", coin_stack.remaining);
		ImGui::Text("Capacity: %d", coin_stack.capacity);

		if(ImGui::Button("TakeCoin"))
		{
			if(ECS::Inventory* inventory = GetComponent(Inventory, Target::GetPlayer()))
			{
				if(coin_stack.remaining > 0)
				{
					coin_stack.remaining--;
					inventory->coins[c_type]++;
				}
			}
		}
		
		if(ImGui::Button("ReturnCoin"))
		{
			if(ECS::Inventory* inventory = GetComponent(Inventory, Target::GetPlayer()))
			{
				if(inventory->coins[c_type] > 0)
				{
					coin_stack.remaining++;
					inventory->coins[c_type]--;
				}
			}
		}

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
		
		ImGui::Text("Points: %d", inventory.GetPoints());

		ImGui::Text("Owned coins");
		for( u32 i = 0; i < ECS::Colour::Count; i++ )
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
		for( u32 i = 0; i < ECS::Colour::Count; i++ )
		{
			SColour colour = ECS::Colour::s_typeToColour.at((ECS::Colour::Type)i);
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