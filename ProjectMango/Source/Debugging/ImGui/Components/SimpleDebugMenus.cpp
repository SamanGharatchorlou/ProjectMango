#include "pch.h"
#include "ComponentDebugMenu.h"

#include "imgui-master/imgui.h"
#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "Debugging/ImGui/ImGuiMenu.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/GameComponents.h"

u32 DebugMenu::DoHealthDebugMenu(ECS::Entity& entity)
{
	StringBuffer32 type_name = Health::TypeName();
	ComponentID type_id = Health::TypeId();

	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		ECS::Health& health = GetComponentRef(Health, entity);
		ImGui::PushID(entity + type_id);

		ImGui::Text("Current: %.f", health.currentHealth);
		ImGui::Text("Max: %.f", health.maxHealth);
				
		if(ImGui::Button("Reduce by 1"))
		{
			health.currentHealth= health.currentHealth - 1.0f;
		}
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

	return type_id;
}

u32 DebugMenu::DoEntityDataDebugMenu(ECS::Entity& entity)
{
	StringBuffer32 type_name = EntityData::TypeName();
	ComponentID type_id = EntityData::TypeId();

	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		ECS::EntityData& entity_data = GetComponentRef(EntityData, entity);
		ImGui::PushID(entity + type_id);

		ECS::EntityManager& em = ecs->entities;
		bool has_parent = entity_data.parent != ECS::EntityInvalid;
		const char* parent = has_parent ? ECS::GetName(entity_data.parent) : "No parent";

		char buffer[32];
		snprintf(buffer, 32, "Parent: %s", parent);
		if (ImGui::ActiveButton(buffer, has_parent))
		{
			DebugMenu::SelectEntity(entity_data.parent);
		}

		for( u32 i = 0; i < entity_data.children.size(); i++ )
		{
			snprintf(buffer, 32, "Child: %s", ECS::GetName(entity_data.children[i]));
			if ( ImGui::ActiveButton(buffer, ecs->IsAlive(entity_data.children[i])) )
			{
				DebugMenu::SelectEntity(entity_data.children[i]);
			}
		}

		ImGui::PopID();
	}

	return type_id;
}

u32 DebugMenu::DoCoinStackDebugMenu(ECS::Entity& entity)
{
	StringBuffer32 type_name = CoinStack::TypeName();
	ComponentID type_id = CoinStack::TypeId();

	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		ECS::CoinStack& coin_stack = GetComponentRef(CoinStack, entity);
		ImGui::PushID(entity + type_id);
		
		SColour col = ECS::Colour::s_typeToColour.at(coin_stack.colourType);
		ECS::Colour::Type c_type = coin_stack.colourType;
		//ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(col.r, col.g, col.b, 255));
		//ImGui::Text("Type: %d", coin_stack.coinType);
		//ImGui::PopStyleColor();

		ImGui::Text("Remaining: %d", coin_stack.remaining);
		ImGui::Text("Capacity: %d", coin_stack.capacity);

		if(ImGui::Button("TakeCoin"))
		{
			if(ECS::Inventory* inventory = GetComponent(Inventory, Faction::GetPlayer()))
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
			if(ECS::Inventory* inventory = GetComponent(Inventory, Faction::GetPlayer()))
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

	return type_id;
}

u32 DebugMenu::DoInventoryDebugMenu(ECS::Entity& entity)
{
	StringBuffer32 type_name = Inventory::TypeName();
	ComponentID type_id = Inventory::TypeId();

	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		ECS::Inventory& inventory = GetComponentRef(Inventory, entity);
		ImGui::PushID(entity + type_id);
		
		ImGui::Text("Points: %d", inventory.GetPoints());

		ImGui::Text("Owned coins");
		for( u32 i = 0; i < ECS::Colour::Count; i++ )
		{
			ImGui::Text("%d: %d", i, inventory.coins[i]);
		}

		ImGui::PopID();
	}

	return type_id;
}

u32 DebugMenu::DoCardDebugMenu(ECS::Entity& entity)
{
	StringBuffer32 type_name = Card::TypeName();
	ComponentID type_id = Card::TypeId();

	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		ECS::Card& card = GetComponentRef(Card, entity);
		ImGui::PushID(entity + type_id);
		
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

	return type_id;
}

u32 DebugMenu::DoFactionDebugMenu(ECS::Entity& entity)
{
	StringBuffer32 type_name = Faction::TypeName();
	ComponentID type_id = Faction::TypeId();

	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		ECS::Faction& faction = GetComponentRef(Faction, entity);
		ImGui::PushID(entity + type_id);

		const char* team = "None";
		if(faction.team == Faction::Player)
			team = "Player";
		if(faction.team == Faction::Enemy)
			team = "Enemy";

		ImGui::Text("%s", team );

		ImGui::PopID();
	}

	return type_id;
}