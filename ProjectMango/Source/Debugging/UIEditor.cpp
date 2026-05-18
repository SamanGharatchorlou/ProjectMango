#include "pch.h"
#include "UiEditor.h"

#include "Core/Helpers.h"
#include "Debugging/ImGui/ImGuiMenu.h"
#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Graphics/RenderManager.h"
#include "Graphics/STexture.h"
#include "Graphics/TextureManager.h"
#include "ImGui/ImGuiHelpers.h"
#include "System/Window.h"
#include "imgui.h"

#include "Input/InputManager.h"
#include "UI/UIManager.h"

namespace UIEditor
{
    static VectorF s_targetWindowSize = VectorF(640, 640);

    struct UIState
    {
        BasicString activeScreen;

        ECS::Entity selectedEntity = ECS::EntityInvalid;
        VectorF cursorOffset;
    };

    static UIState s_state;

    static bool UIEntityBuilder()
    {
        bool did_add = false;
        if (ImGui::TreeNode("Entity Builder"))
        {
            UIManager& ui_manager = UIManager::Get();

            if (ImGui::Button("create quad"))
            {
                ECS::EntityMetaData meta_data;

                //transform
                meta_data.data.strings["Id"] = "quad";
                meta_data.data.vectors["Size"] = VectorF(100.0f, 100.0f);
                meta_data.data.vectors["Position"] = VectorF(100.0f, 100.0f);

                // sprite
                meta_data.data.strings["Sprite"] = "EditorBg_black";
                
                // add to screen meta data
                ui_manager.screenMetaData[s_state.activeScreen].push_back(meta_data);
                
                did_add = true;
            }

            ImGui::TreePop();
        }

        return did_add;
    }

    static ECS::Entity SelectableUIEntityList()
    {
        ECS::Entity selected_entity = s_state.selectedEntity;

        UIManager& ui_manager = UIManager::Get();
        UIScreenEntities& screen_entities = ui_manager.screenEntities[s_state.activeScreen];

        char buffer[64];
        snprintf(buffer, 64, "Screen Entities: %s", ECS::GetName(s_state.selectedEntity));
        if (ImGui::TreeNode(buffer))
        {
            for (u32 i = 0; i < screen_entities.size(); i++)
            {
                ImGui::PushID(i);
                if (ImGui::Button(ECS::GetName(screen_entities[i])))
                {
                    selected_entity = screen_entities[i];
                }
                if (s_state.selectedEntity == screen_entities[i])
                {
                    ImGui::SameLine(); ImGui::Text(" <--");
                }
                ImGui::PopID();
            }
            ImGui::TreePop();
        }

        return selected_entity;
    }

    static void UIEntityControls()
    {
        ImGui::Begin("Entity Controls", nullptr, 0);

        ImGui::Text("Click-drag to reposition");
        ImGui::Text("Hold-Space and using arrows to resize");

        ECS::Transform& transform = GetComponentRef(Transform, s_state.selectedEntity);

        // input values
        float pos[2]{ transform.worldPosition.x, transform.worldPosition.y };
        if(ImGui::InputFloat2("World Pos", pos))
            transform.worldPosition = VectorF(pos[0], pos[1]);

        float size[2]{ transform.size.x, transform.size.y };
        if (ImGui::InputFloat2("Size", size))
            transform.size = VectorF(size[0], size[1]);

        // input controls
        InputManager* im = GameData::Get().inputManager;
        bool is_held = false;

        // edit quad position
        if (im->isCursorHeld(Cursor::Left) && !s_state.cursorOffset.isZero())
        {
            const VectorF cursor_pos = im->cursorScreenPosition();
            if (Contains(transform.GetRect(), cursor_pos))
            {
                is_held = true;
                transform.SetWorldPosition(cursor_pos + s_state.cursorOffset);
            }
        }

        // edit quad size
        if (im->isHeld(Button::Space))
        {
            VectorF& size = transform.size;
            if (im->isHeld(Button::UpArrow))
                size.y += 0.5;
            if (im->isHeld(Button::DownArrow))
                size.y -= 0.5;

            if (im->isHeld(Button::RightArrow))
                size.x += 0.5;
            if (im->isHeld(Button::LeftArrow))
                size.x -= 0.5;
        }

        DebugDraw::RectOutline(transform.GetRect(), is_held ? SColour::Green : SColour::Yellow);

        ImGui::End();
    }

    static ECS::Entity GetEntityAtCursor()
    {
        InputManager* im = GameData::Get().inputManager;
        if (im->isCursorPressed(Cursor::Left))
        {
            const VectorF cursor_pos = im->cursorScreenPosition();
            const UIScreenEntities& screen_entities = UIManager::Get().screenEntities[s_state.activeScreen];
            for (u32 i = 0; i < screen_entities.size(); i++)
            {
                ECS::Transform& transform = GetComponentRef(Transform, screen_entities[i]);
                if (Contains(transform.GetRect(), cursor_pos))
                    return screen_entities[i];
            }
        }

        return ECS::EntityInvalid;
    }

    void Update()
    {
        ImGui::Begin("UI Editor", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize);

        UIManager& ui_manager = UIManager::Get();
        InputManager* im = GameData::Get().inputManager;

        // refresh screen entities when something was added
        if (UIEntityBuilder())
        {
            ui_manager.CloseScreen(s_state.activeScreen.c_str());
            ui_manager.OpenScreen(s_state.activeScreen.c_str());
        }

        s_state.selectedEntity = SelectableUIEntityList();

        // while we dont have anything selected click to select something
        if (!ecs->IsAlive(s_state.selectedEntity))
        {
            s_state.selectedEntity = GetEntityAtCursor();
        }
        // handle controls for positioning/sizing etc.
        else
        {
            // set cursor offset from quad and cursor
            if (im->isCursorPressed(Cursor::Left))
            {
                const VectorF cursor_pos = im->cursorScreenPosition();
                ECS::Transform& transform = GetComponentRef(Transform, s_state.selectedEntity);
                s_state.cursorOffset = transform.worldPosition - cursor_pos;
            }
            if (im->isCursorReleased(Cursor::Left))
            {
                s_state.cursorOffset = VectorF::zero();
            }

            UIEntityControls();
        }

        if (im->isCursorPressed(Cursor::Right))
            s_state.selectedEntity = ECS::EntityInvalid;

        ImGui::End();
    }

    void Render()
    {
        const VectorF window_size = GameData::Get().window->size();
        RenderManager* rm = GameData::Get().renderManager;

        RectF screen(VectorF::zero(), s_targetWindowSize);
        STexture* black_bg = TextureManager::Get()->getTexture("EditorBg_black", FileManager::Image_UI);
        STexture* white_bg = TextureManager::Get()->getTexture("EditorBg", FileManager::Image_UI);
        RenderPack pack(white_bg, 0);
        pack.rect = screen;
        rm->AddRenderPacket(pack);
    }

    static void AddTestScreen(const char* screen)
    {
        UIManager& ui_manager = UIManager::Get();

        s_state.activeScreen = screen;
        ui_manager.screenMetaData[s_state.activeScreen];
    }

    bool IsOpen()
    {
        return DebugMenu::IsUIEditorActive();
    }

    void Open()
    {
        DebugMenu::ToggleUIWindow(true);
        AddTestScreen("debug screen");
    }

    void Close()
    {
        DebugMenu::ToggleUIWindow(false);
    }
}