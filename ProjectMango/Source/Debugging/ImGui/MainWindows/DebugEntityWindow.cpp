#include "pch.h"

#include "imgui-master/imgui.h"
#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiMenu.h"
#include "Input/InputManager.h"
#include "Core/Helpers.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "Debugging/ImGui/Components/ComponentDebugMenu.h"

#define DoRemoveButton(type) \
    ImGui::PushID(id_numb++); do_dropdown = true; \
    if(ImGui::Button("-")) {\
        RemoveComponent(type, DebugMenu::GetSelectedEntity()); do_dropdown = false; }\
    if(ImGui::IsItemHovered()) \
        ImGui::SetTooltip("%s", ECS::type::TypeName()); \
    ImGui::SameLine(); ImGui::PopID(); \

#define ComponentDropdown(menu) \
    if(do_dropdown) \
        SetFlag<u64>(type, ECS::archetypeBit((ComponentID)menu(DebugMenu::GetSharedState().selectedEntity)));

#define DoComponentView(component) \
    SetFlag<u64>(type, ECS::archetypeBit(component::TypeId())); \
    ComponentArray<component>& components = GetAllComponents(component); \
    ImGui::PushID(id_numb++); \
    ImGui::Text("%s: %d", components.TypeName(), components.Count() ); \
    ImGui::PopID(); \


#define DoComponentDropdown(component) \
    if(entity_view) { \
        if(HasComponent(component, DebugMenu::GetSelectedEntity())) { \
            DoRemoveButton(component); ComponentDropdown(Do##component##DebugMenu); } } \
    else { \
        DoComponentView(component); } \

#define DoComponentTestView(component_name) \
    ComponentArray<component>& components = GetAllComponents(component_name); \
    ImGui::PushID(id_numb++); \
    ImGui::Text("%s: %d", component_name, components.Count()); \
    ImGui::PopID(); \


namespace DebugMenu
{
    struct EntityPartState
    {
        bool ignoreTerrain = true;
        bool hideDropdowns = false;
        StringBuffer64 filterBuffer;

        //int idNumb = 0;
        bool entityView = true;

    };

    static EntityPartState s_state;

    // Entity Window
    void DoEntityPartSystemWindow(bool entity_view)
    {
        using namespace ECS;

        ImGui::Begin("Entity Window", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize);

        EntityManager& em = ecs->entities;
        Entity& selected_entity = GetSharedState().selectedEntity;

        StringBuffer64& filter = s_state.filterBuffer;
        ImGui::Text("Selected Entity: %d", (int)selected_entity);
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


        //if (is_number)
        //{
        //    int number = std::atoi(filter.c_str());
        //    if (ecs->IsAlive(number))
        //    {
        //        selected_entity = number;
        //    }
        //}

        if (!ecs->IsAlive(selected_entity))
        {
            ImGui::Text("Entity %d is dead", selected_entity);
        }

        const char* selected = ECS::GetName(selected_entity);
        if (!selected)
            selected = "";

        UICursor* cursor = UICursor::Get();
        InputManager* input = InputManager::Get();
        VectorF cursor_pos = input->cursorWorldPosition();
        const ComponentArray<Transform>& transforms = GetAllComponents(Transform);
        for (auto iter = transforms.entityToComponent.begin(); iter != transforms.entityToComponent.end(); iter++)
        {
            Entity entity = iter->first;
            if (s_state.ignoreTerrain && IsTerrain(entity))
            {
                continue;
            }

            if (entity == cursor->entity)
                continue;

            const Transform& transform = transforms.GetComponentByIndex(iter->second);
            if (Contains(transform.GetObjectRect(), cursor_pos))
            {
                ImGui::Text("Hovered Entity: %s(%d)", GetName(entity), entity);
                break;
            }
        }

        ImGui::Checkbox("Ignore Terrain Entities", &s_state.ignoreTerrain);

        if (ImGui::BeginCombo("Entities", selected, 0))
        {
            const ECS::ComponentArray<ECS::EntityData>& entity_data = GetAllComponents(EntityData);
            for (auto iter = entity_data.entityToComponent.begin(); iter != entity_data.entityToComponent.end(); iter++)
            {
                const  ECS::EntityData& ed = entity_data.GetComponentByIndex(iter->second);
                StringBuffer64 entity_name = StringBuffer64(ed.id.c_str()).to_lower();

                if (is_number)
                {
                    int number = std::atoi(filter.c_str());
                    if (iter->first != number)
                        continue;
                }
                else if (filter.length() > 0 )
                {
                    StringBuffer64 filter_lower = filter.to_lower();
                    const char* value = strstr(entity_name.c_str(), filter_lower.c_str());
                    if (!value)
                        continue;
                }

                if (s_state.ignoreTerrain && IsTerrain(ed.entity))
                {
                    continue;
                }

                ImGui::PushID(iter->first);

                const bool is_selected = iter->first == selected_entity;
                if (ImGui::Selectable(entity_name.c_str(), is_selected))
                    selected_entity = iter->first;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();

                ImGui::PopID();
            }

            ImGui::EndCombo();
        }

        ImGui::Checkbox("Hide dropdowns", &s_state.hideDropdowns);

        if (!s_state.hideDropdowns && ecs->IsAlive(selected_entity))
        {
            if (const ECS::Transform* transform = GetComponent(Transform, selected_entity))
            {
                RectF rect(transform->worldPosition, transform->size);
                DebugDraw::RectOutline(rect, SColour::Blue);
            }

            ECS::Health* health = GetComponent(Health, selected_entity);
            if (ImGui::ActiveButton("Kill Entity", health != nullptr))
            {
                health->currentHealth = 0;
            }

            ImGui::SameLine();
            if (ImGui::Button("Destroy Entity"))
            {
                em.KillEntity(selected_entity);
                ImGui::End();
                return;
            }

            int id_numb = 0;
            ECS::Archetype type = 0;
            bool do_dropdown = true;

            DoComponentDropdown(EntityData);
            DoComponentDropdown(Animator);
            DoComponentDropdown(Collider);
            DoComponentDropdown(EntityState);
            DoComponentDropdown(Physics);
            DoComponentDropdown(Sprite);
            DoComponentDropdown(Transform);
            DoComponentDropdown(Pathing);
            DoComponentDropdown(AIController);
            DoComponentDropdown(Health);
            DoComponentDropdown(Biome);
            DoComponentDropdown(UIButton);
            DoComponentDropdown(UIText);
            DoComponentDropdown(CoinStack);
            DoComponentDropdown(Inventory);
            DoComponentDropdown(Card);
            DoComponentDropdown(BehaviourState);
            DoComponentDropdown(Faction);
            DoComponentDropdown(AIIntent);

            // for menus we havent defined yet at least show something
            if (entity_view)
            {
                ECS::Archetype entity_type = em.GetAchetype(selected_entity);
                for (u32 i = 0; i < ComponentCount; i++)
                {
                    if (entity_type & ECS::archetypeBit(i))
                    {
                        if (type & ECS::archetypeBit(i))
                            continue;

                        ImGui::Button("-");
                        ImGui::SameLine();
                        ImGui::Text(ECS::ComponentNames[i]);
                    }
                }
            }
            else
            {
                for (u32 i = 0; i < ComponentCount; i++)
                {
                    if (type & ECS::archetypeBit(i))
                        continue;

                    const char* name = ecs->components.componentArrays[i]->TypeName();
                    u32 count = ecs->components.componentArrays[i]->Count();

                    ImGui::Text("%s: %d", name, count);
                }
            }
        }

        ImGui::End();
    }
}