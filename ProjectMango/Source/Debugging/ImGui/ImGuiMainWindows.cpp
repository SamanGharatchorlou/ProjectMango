#include "pch.h"
#include "ImGuiMainWindows.h"

#include "imgui-master/imgui.h"
#include "ImGuiHelpers.h"
#include "Core/Helpers.h"

#include "ECS/EntityManager.h"
#include "ECS/EntityCoordinator.h"
#include "Graphics/RenderManager.h"
#include "Input/InputManager.h"
#include "Game/FrameRateController.h"
#include "ECS/Components/ComponentsSetup.h"
#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"

#include "ECS/Components/AIController.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/PlayerController.h"
#include "ECS/Components/TileMap.h"
#include "Debugging/ImGui/Components/ComponentDebugMenu.h"
#include "ECS/Components/Animator.h"

#include "Characters/Player/PlayerCharacter.h"
#include "Characters/Spawner.h"

ECS::Entity s_selectedEntity = 0;

u32 DebugMenu::GetSelectedEntity() { return s_selectedEntity; }
void DebugMenu::SelectEntity(ECS::Entity entity) { s_selectedEntity = entity; }

int id_numb = 0;

#define DoRemoveButton(type) \
    ImGui::PushID(id_numb++); do_dropdown = true; \
    if(ImGui::Button("-")) {\
        ecs->RemoveComponent(type, s_selectedEntity); do_dropdown = false; }\
    if(ImGui::IsItemHovered()) \
        ImGui::SetTooltip("%s", ECS::ComponentNames[(int)ECS::Component::type]); \
    ImGui::SameLine(); ImGui::PopID(); \

#define ComponentDropdown(menu) \
    if(do_dropdown) \
        SetFlag<u64>(type, ECS::archetypeBit(menu(s_selectedEntity)));

#define DoComponentDropdown(component) \
    if(ecs->HasComponent(component,s_selectedEntity)) {\
        DoRemoveButton(component); ComponentDropdown(Do##component##DebugMenu); }

// Entity Window
void DebugMenu::DoEntitySystemWindow()
{
    ImGui::Begin("Entity Window", nullptr, ImGuiWindowFlags_MenuBar);
    
    ECS::EntityCoordinator* ecs = GameData::Get().ecs;
    ECS::EntityManager& em = ecs->entities;
    std::unordered_map<ECS::Entity, BasicString>& entityNames = em.entityNames;

    const char* selected = entityNames.count(s_selectedEntity) > 0 ? entityNames[s_selectedEntity].c_str() : "No selection";

    ImGui::Text("Selected Entity: %d", (int)s_selectedEntity);

    if (ImGui::BeginCombo("Entities", selected, 0))
    {
        for (auto iter = entityNames.begin(); iter != entityNames.end(); iter++)
        {
            const bool is_selected = iter->first == s_selectedEntity;

            if (ImGui::Selectable(iter->second.c_str(), is_selected))
                s_selectedEntity = iter->first;

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

    if (GameData::Get().ecs->IsAlive(s_selectedEntity)) 
    {
        if (ecs->IsAlive(s_selectedEntity))
        {
			ECS::Health* health = ecs->GetComponent(Health, s_selectedEntity);
            if(ImGui::ActiveButton("Kill Entity", health != nullptr))
            {
                health->currentHealth = 0;
            }

            if(ImGui::Button("Destroy Entity"))
            {
                GameData::Get().ecs->entities.KillEntity(s_selectedEntity);
                ImGui::End();
                return;
            }

            id_numb = 0;
            ECS::Archetype type = 0;
            bool do_dropdown = true;

            DoComponentDropdown(Animator);
            //DoComponentDropdown(TileMap);
            DoComponentDropdown(Collider);
            DoComponentDropdown(CharacterState);
            DoComponentDropdown(Physics);
            DoComponentDropdown(Sprite);
            DoComponentDropdown(Transform);
            DoComponentDropdown(Pathing);
            DoComponentDropdown(AIController);
            DoComponentDropdown(PlayerController);
            DoComponentDropdown(Health);

            ECS::Archetype entity_type = ecs->entities.GetAchetype(s_selectedEntity);
            for (u32 i = 0; i < ECS::Component::Count; i++) 
            {
                if(entity_type & ECS::archetypeBit((ECS::Component::Type)i))
                {
                    if(type & ECS::archetypeBit((ECS::Component::Type)i))
                        continue;

                    ImGui::Button("-");
                    ImGui::SameLine();
		            ImGui::Text(ECS::ComponentNames[i]);
                }
            }
        }
    }

    ImGui::End();
}

void DebugMenu::DoInputWindow()
{
    ImGui::Begin("Input Window", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::CollapsingHeader("Inputs & Focus"))
    {
        ImGuiIO& io = ImGui::GetIO();

        // Display inputs submitted to ImGuiIO
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("ImGui Inputs"))
        {
            if (ImGui::IsMousePosValid())
                ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
            else
                ImGui::Text("Mouse pos: <INVALID>");
            ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
            ImGui::Text("Mouse down:");
            for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseDown(i)) { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
            ImGui::Text("Mouse wheel: %.1f", io.MouseWheel);

            // We iterate both legacy native range and named ImGuiKey ranges, which is a little odd but this allows displaying the data for old/new backends.
            // User code should never have to go through such hoops! You can generally iterate between ImGuiKey_NamedKey_BEGIN and ImGuiKey_NamedKey_END.
#ifdef IMGUI_DISABLE_OBSOLETE_KEYIO
            struct funcs { static bool IsLegacyNativeDupe(ImGuiKey) { return false; } };
            ImGuiKey start_key = ImGuiKey_NamedKey_BEGIN;
#else
            struct funcs { static bool IsLegacyNativeDupe(ImGuiKey key) { return key < 512 && ImGui::GetIO().KeyMap[key] != -1; } }; // Hide Native<>ImGuiKey duplicates when both exists in the array
            ImGuiKey start_key = (ImGuiKey)0;
#endif
            ImGui::Text("Keys down:");         for (ImGuiKey key = start_key; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1)) { if (funcs::IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key)) continue; ImGui::SameLine(); ImGui::Text((key < ImGuiKey_NamedKey_BEGIN) ? "\"%s\"" : "\"%s\" %d", ImGui::GetKeyName(key), key); }
            ImGui::Text("Keys mods: %s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");
            ImGui::Text("Chars queue:");       for (int i = 0; i < io.InputQueueCharacters.Size; i++) { ImWchar c = io.InputQueueCharacters[i]; ImGui::SameLine();  ImGui::Text("\'%c\' (0x%04X)", (c > ' ' && c <= 255) ? (char)c : '?', c); } // FIXME: We should convert 'c' to UTF-8 here but the functions are not public.

            ImGui::TreePop();
        }
        if (ImGui::TreeNode("SDL Inputs"))
        {
            InputManager* im = InputManager::Get();

            StringBuffer32 cursorState = "None";
            if (im->mCursor.isPressed())
                cursorState = "Pressed";
            if (im->mCursor.isReleased())
                cursorState = "Released";
            else if (im->mCursor.isHeld())
                cursorState = "Held";

            ImGui::Text("Cursor %s Moving: ", im->mCursor.isMoving() ? "is" : "is not");
            ImGui::Text("Cursor: %s", cursorState.c_str());

            const FrameRateController& frc = FrameRateController::Get();
	        const int frame_count = frc.frameCount();

            for (u32 i = 0; i < im->mButtons.size(); i++)
            {
                Button& button = im->mButtons[i];

                StringBuffer32 state;

                if (button.isPressed(frame_count))
                    state = "Pressed, ";
                if (button.isHeld())
                    state = state + "Held, ";
                if (button.isReleased(frame_count))
                    state = state + "Released";

                ImGui::PushID(i);
                if (!state.empty())
                {
                    ImGui::Text("Button %d: %s || Held Frames %d", i, state.c_str(), button.getHeldFrames());
                }
                ImGui::PopID();
            }

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

static bool s_displayStatics = true;
static bool s_displayDynamics = true;
static DebugDrawType s_drawType = DebugDrawType::RectOutline;

void DebugMenu::DoColliderWindow() 
{
    ECS::EntityCoordinator* ecs = GameData::Get().ecs;

	ECS::ComponentArray<ECS::Collider>& colliders =  ecs->GetComponents<ECS::Collider>(ECS::Component::Type::Collider);
	std::vector<ECS::Collider>& collider_list = colliders.components;

    ImGui::Checkbox("Display Statics", &s_displayStatics);
    ImGui::Checkbox("Display Dynamics", &s_displayDynamics);

    ImGui::DoDebugRenderTypeDropDown(s_drawType);

    // first we need to update the collider position with where the entity wants to be
    for(u32 i = 0; i < collider_list.size(); i++)
	{
        const ECS::Collider& collider = collider_list[i];

		// ignore static colliders, they dont move
        bool is_static = HasFlag(collider.mFlags, ECS::Collider::Flags::Static);
		if(!s_displayStatics && is_static)
            continue;
        if(!s_displayDynamics && !is_static)
            continue;

        Colour colour = Colour::Blue;
        if(is_static)
        {
            colour = Colour::Purple;
        }

       bool ignore_collisions = HasFlag(collider.mFlags, ECS::Collider::Flags::IgnoreCollisions);
        if(ignore_collisions)
        {
            colour = Colour::Black;
        }

        bool ignore_all = HasFlag(collider.mFlags, ECS::Collider::Flags::IgnoreAll);
        if(ignore_all)
        {
            colour = Colour::LightGrey;
            colour.a = 100;
        }

        DebugDraw::Shape(s_drawType, collider.GetRect(), colour);
	}
}

void DebugMenu::DoGameStateWindow() 
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;

    if(ImGui::Button("Restart Game State"))
    {
        GameData::Get().systemStateManager->mStates.replaceState(new GameState);
    }
}

static bool s_playerInvulnerable = true;

void DebugMenu::DoTweakerWindow() 
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Entity entity = Player::Get();

    ImGui::Checkbox("Player Invulnerable", &s_playerInvulnerable);

    ECS::Health& health = ecs->GetComponentRef(Health, entity);
    health.invulnerable = s_playerInvulnerable;
}