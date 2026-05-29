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

struct EntityDataState
{
	StringBuffer64 filterBuffer;
	Entity selected = EntityInvalid;
};

static EntityDataState s_state;

u32 DebugMenu::DoEntityDataDebugMenu(ECS::Entity& entity)
{
	COMPONENT_PREAMBLE(EntityData);

	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		ECS::EntityData& entity_data = GetComponentRef(EntityData, entity);
		ImGui::PushID(entity + type_id);

		ImGui::Text("UID: (0x%08X)", entity_data.iid);
		ImGui::SameLine();
		if (ImGui::Button("Regenerate UID"))
		{
			entity_data.iid = Maths::randomNumberBetween(0, INT_MAX);
		}

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


		StringBuffer64& filter = s_state.filterBuffer;
		ImGui::InputText("Entity Filter", filter.buffer(), filter.bufferLength());

		bool is_number = filter.length() > 0;
		for (u32 i = 0; i < filter.length(); i++)
		{
			char* c = filter.buffer() + i;
			int value = *c;
			if (!std::isdigit(value))
			{
				is_number = false;
				break;
			}
		}

		const char* selected = ECS::GetName(s_state.selected);
		if (!selected)
			selected = "";

		if (ImGui::BeginCombo("Add child", selected, 0))
		{
			int number = std::atoi(filter.c_str());

			const ECS::ComponentArray<ECS::EntityData>& entity_data = GetAllComponents(EntityData);
			for (auto iter = entity_data.entityToComponent.begin(); iter != entity_data.entityToComponent.end(); iter++)
			{
				const  ECS::EntityData& ed = entity_data.GetComponentByIndex(iter->second);
				StringBuffer64 entity_name = StringBuffer64(ed.id.c_str()).to_lower();

				if (is_number && iter->first != number)
				{
					continue;
				}
				else if (filter.length() > 0 && !is_number)
				{
					StringBuffer64 filter_lower = filter.to_lower();
					const char* value = strstr(entity_name.c_str(), filter_lower.c_str());
					if (!value)
						continue;
				}

				const bool is_selected = iter->first == s_state.selected;
				if (ImGui::Selectable(entity_name.c_str(), is_selected))
				{
					s_state.selected = iter->first;

					EntityData::SetParent(iter->first, entity);
				}
			}

			ImGui::EndCombo();
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
			const char* colour_string = Colour::s_typeToString.at((Colour::Type)i).c_str();
			ImGui::Text("%s: %d", colour_string, inventory.coins[i]);
		}

		for (u32 i = 0; i < inventory.relics.size(); i++)
		{
			ImGui::Text(inventory.relics[i].id.c_str());
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

ComponentID DebugMenu::DoCallbackDebugMenu(ECS::Entity& entity)
{
	COMPONENT_PREAMBLE(Callback);

	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		ECS::Callback& callback = GetComponentRef(Callback, entity);
		ImGui::PushID(entity + type_id);

		StringBuffer64 callback_input = callback.callback.c_str();
		if (ImGui::InputText("Callback", callback_input.buffer(), callback_input.bufferLength()))
		{
			callback.callback = callback_input.c_str();
		}

		ImGui::PopID();
	}

	return type_id;
}