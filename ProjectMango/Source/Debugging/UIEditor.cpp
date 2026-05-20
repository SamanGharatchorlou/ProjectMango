#include "pch.h"
#include "UiEditor.h"

#include "Core/Helpers.h"
#include "Debugging/ImGui/Components/ComponentDebugMenu.h"
#include "Debugging/ImGui/ImGuiMenu.h"
#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Entities/Factory/EntityBuilder.h"
#include "Graphics/RenderManager.h"
#include "Graphics/STexture.h"
#include "Graphics/TextureManager.h"
#include "Input/InputManager.h"
#include "System/Window.h"
#include "UI/UIManager.h"
#include "imgui.h"
#include "Entities/Factory/EntitySerialiser.h"

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

    static void UIEntityBuilder()
    {
        if (ImGui::TreeNode("Entity Builder"))
        {
            UIManager& ui_manager = UIManager::Get();

            if (ImGui::Button("create quad"))
            {
                ECS::EntityMetaData meta_data;

                static int index = 0;

                char buffer[32];
                snprintf(buffer, 32, "element %d", ++index);

                //transform
                meta_data.data.strings["Id"] = buffer;
                meta_data.data.vectors["Size"] = VectorF(100.0f, 100.0f);
                meta_data.data.vectors["Position"] = VectorF(100.0f, 100.0f);

                // sprite
                meta_data.data.strings["Sprite"] = "EditorBg_black";
                
                // add to screen meta data
                ui_manager.screenMetaData[s_state.activeScreen].push_back(meta_data);

                Entity entity = CreateEntityFromData(meta_data);
                ui_manager.screenEntities[s_state.activeScreen].push_back(entity);
            }

            ImGui::TreePop();
        }
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
                ImGui::SameLine();

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

    static void UITransformEditor(ECS::Entity entity)
    {
        ECS::Transform& transform = GetComponentRef(Transform, entity);

        ImGui::Text("Click-drag to reposition");
        ImGui::Text("Hold-Shift and using arrows to resize");

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
        if (im->isHeld(Button::Shift))
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
    }

    static void UISpriteEditor(ECS::Entity entity)
    {
        if (ECS::Sprite* sprite = GetComponent(Sprite, entity))
        {
            if (ImGui::Button("Remove Sprite"))
            {
                RemoveComponent(Sprite, entity);
            }

            DebugMenu::DoSpriteDebugMenu(entity);
        }
        else
        {
            if (ImGui::Button("Add Sprite"))
            {
                AddComponent(Sprite, entity);
            }
        }
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

    static void CursorControls(ECS::Entity entity)
    {
        ECS::Transform& transform = GetComponentRef(Transform, entity);

        // set cursor offset from quad and cursor
        InputManager* im = GameData::Get().inputManager;
        if (im->isCursorPressed(Cursor::Left))
        {
            const VectorF cursor_pos = im->cursorScreenPosition();
            s_state.cursorOffset = transform.worldPosition - cursor_pos;
        }
        if (im->isCursorReleased(Cursor::Left))
        {
            s_state.cursorOffset = VectorF::zero();
        }

        // input controls
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
    }

    void Update()
    {
        ImGui::Begin("UI Editor", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize);

        UIManager& ui_manager = UIManager::Get();
        InputManager* im = GameData::Get().inputManager;

        if (ImGui::Button("Save Entities to File"))
        {
            std::vector<ECS::EntityMetaData*> meta_data;

            UIScreenMetaData& meta_datas = ui_manager.screenMetaData[s_state.activeScreen];
            meta_datas.clear();

            UIScreenEntities& entities = ui_manager.screenEntities[s_state.activeScreen];
            for (u32 i = 0; i < entities.size(); i++)
            {
                meta_datas.push_back(ECS::EntityMetaData());
                ECS::EntityMetaData& md = meta_datas.back();

                ReadMetaDataFromEntity(entities[i], md);
            }

            const UIScreenMetaData& screen_metas = ui_manager.screenMetaData[s_state.activeScreen];

            LevelSizeInfo size_info;
            size_info.windowToLevel = GameData::Get().window->windowToLevel;

            const char* fp = "C:/Users/saman/Documents/Code/ProjectMango/ProjectMango/Resources/Maps/data.dat";
            SaveEntityToJson(fp, size_info, screen_metas);
        }

        ImGui::SameLine();
        if (ImGui::Button("Load Entities from File"))
        {
            LevelSizeInfo size_info;
            size_info.windowToLevel = GameData::Get().window->windowToLevel;

            const char* fp = "C:/Users/saman/Documents/Code/ProjectMango/ProjectMango/Resources/Maps/data.dat";

            UIScreenEntities& screen_entities = ui_manager.screenEntities[s_state.activeScreen];
            for (u32 i = 0; i < screen_entities.size(); i++)
            {
                ecs->entities.KillEntity(screen_entities[i]);
            }
            screen_entities.clear();

            UIScreenMetaData& screen_metas = ui_manager.screenMetaData[s_state.activeScreen];
            LoadEntityFromJson(fp, size_info, screen_metas);

            ui_manager.CloseScreen(s_state.activeScreen.c_str());
            ui_manager.OpenScreen(s_state.activeScreen.c_str());
        }

        if (ImGui::Button("Regenerate Entities from MetaData"))
        {
            ui_manager.CloseScreen(s_state.activeScreen.c_str());
            ui_manager.OpenScreen(s_state.activeScreen.c_str());
        }

        UIEntityBuilder();

        s_state.selectedEntity = SelectableUIEntityList();

        // while we dont have anything selected click to select something
        if (!ecs->IsAlive(s_state.selectedEntity))
        {
            s_state.selectedEntity = GetEntityAtCursor();
        }
        // handle controls for positioning/sizing etc.
        else
        {
            ImGui::Begin("Entity Controls", nullptr, 0);

            CursorControls(s_state.selectedEntity);
            DebugMenu::DoTransformDebugMenu(s_state.selectedEntity);
            DebugMenu::DoSpriteDebugMenu(s_state.selectedEntity);
            DebugMenu::DoUITextDebugMenu(s_state.selectedEntity);

            ImGui::End();
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