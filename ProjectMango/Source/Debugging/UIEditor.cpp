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
#include "Entities/Factory/ComponentAssembler.h"

namespace UIEditor
{
    static VectorF s_targetWindowSize = VectorF(640, 640);

    struct UIState
    {
        BasicString activeScreen;
        StringBuffer64 activeScreenInput;

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
                meta_data.data.AddString("id", buffer);
                meta_data.data.AddVectorF("size", VectorF(100.0f, 100.0f));
                meta_data.data.AddVectorF("position", VectorF(100.0f, 100.0f));

                // sprite
                meta_data.data.strings["sprite"] = "EditorBg_black";
                
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
        ECS::Entity selected_entity = DebugMenu::GetSelectedEntity();

        UIManager& ui_manager = UIManager::Get();
        UIScreenEntities& screen_entities = ui_manager.screenEntities[s_state.activeScreen];

        const char* name = GetName(selected_entity);

        char buffer[64];
        snprintf(buffer, 64, "Screen Entities: %s", name ? name : "");
        if (ImGui::TreeNodeEx(buffer, ImGuiTreeNodeFlags_DefaultOpen))
        {
            for (u32 i = 0; i < screen_entities.size(); i++)
            {
                Entity entity = screen_entities[i];
                const char* ent_name = GetName(entity);

                if (EntityData* ed = GetComponent(EntityData, entity))
                {
                    ImGui::PushID(entity);
                    if (ImGui::Button(ent_name))
                    {                        
                        selected_entity = entity;
                    }
                    if (DebugMenu::GetSelectedEntity() == entity)
                    {
                        ImGui::SameLine(); ImGui::Text(" <--");
                    }
                    ImGui::PopID();
                }
            }
            ImGui::TreePop();
        }

        return selected_entity;
    }

    static ECS::Entity GetEntityAtCursor(Entity selected_entity)
    {
        InputManager* im = GameData::Get().inputManager;
        if (im->isCursorPressed(Cursor::Left))
        {
            const VectorF cursor_pos = im->cursorScreenPosition();

            const UIManager& ui_manager = UIManager::Get();
            const UIScreenEntities& screen_entities = ui_manager.screenEntities.at(s_state.activeScreen);
            for (u32 i = 0; i < screen_entities.size(); i++)
            {
                Entity target_entity = screen_entities[i];
                const Transform& tgt_transform = GetComponentRef(Transform, target_entity);
                if (Contains(tgt_transform.GetRect(), cursor_pos))
                {
                    RenderLayer target_layer = RenderLayer::None;
                    if (const Sprite* target_sprite = GetComponent(Sprite, target_entity))
                        target_layer = target_sprite->params.renderLayer;

                    RenderLayer selected_layer = RenderLayer::None;
                    if (const Transform* selected_transform = GetComponent(Transform, selected_entity))
                    {
                        if (Contains(selected_transform->GetRect(), cursor_pos))
                        {
                            if (const Sprite* selected_sprite = GetComponent(Sprite, selected_entity))
                                selected_layer = selected_sprite->params.renderLayer;
                        }
                    }

                    if (target_layer > selected_layer)
                        selected_entity = target_entity;
                }
            }

            return selected_entity;
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

    static void DoEntityControlPanel(Entity entity, UIScreenMetaData& screen_metas, UIScreenEntities& entities)
    {
        ImGui::Begin("Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        if (EntityData* ed = GetComponent(EntityData, entity))
        {
            char text_buffer[64];
            snprintf(text_buffer, 64, "%s", ed->id.c_str());
            if (ImGui::InputText("ID", text_buffer, 64))
            {
                ed->id = text_buffer;
            }
        }

        if (ImGui::Button("Duplicate"))
        {
            for (u32 i = 0; i < entities.size(); i++)
            {
                if (entity == entities[i]) 
                {
                    EntityMetaData emd = screen_metas[i];

                    char buffer[64]{ 0 };
                    snprintf(buffer, 64, "%s - copy", emd.GetID());

                    emd.data.AddString("id", buffer);
                    emd.data.AddVectorF("position", VectorF::zero());
                    emd.data.AddU64("iid", Maths::GenerateIID());

                    screen_metas.push_back(emd);

                    Entity new_entity = CreateEntityFromData(emd);
                    entities.push_back(new_entity);
                }
            }
        }

        if (entity != EntityInvalid)
        {
            CursorControls(entity);
            DebugMenu::DoUIEditorMenus(entity);
        }

        ImGui::End();
    }

    void Update()
    {
        ImGui::Begin("UI Editor", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize);

        UIManager& ui_manager = UIManager::Get();
        InputManager* im = GameData::Get().inputManager;

        if (ImGui::Button("Open new screen:"))
        {
            for (auto& [key, val] : ui_manager.screenEntities)
            {
                ui_manager.CloseScreen(key.c_str());
            }

            const char* screen = s_state.activeScreen.c_str();
            ui_manager.screenEntities[screen];
            ui_manager.screenMetaData[screen];
        }

        StringBuffer64 active_screen = s_state.activeScreen.c_str();
        ImGui::SameLine();
        if (ImGui::InputText(" ", active_screen.buffer(), active_screen.bufferLength()))
        {
            s_state.activeScreen = active_screen.c_str();
        }

        int selected = -1;
        std::vector<const char*> keys;
        for (auto& [key, val] : ui_manager.screenMetaData)
        {
            if (key == s_state.activeScreen)
                selected = (int)keys.size();

            keys.push_back(key.c_str());
        }
        bool force_load_entities_from_file = false;
        if (ImGui::Combo("Set active screen", &selected, keys.data(), (int)keys.size()))
        {
            s_state.activeScreen = keys[selected];

            for (auto& [key, val] : ui_manager.screenEntities)
            {
                ui_manager.CloseScreen(key.c_str());
            }

            force_load_entities_from_file = true;
        }

        const char* screen = s_state.activeScreen.c_str();
        if (!ui_manager.screenMetaData.contains(screen))
        {
            ImGui::Text("Need to open this as a new screen");
            ImGui::End();
            return;
        }

        UIScreenEntities& entities = ui_manager.screenEntities[screen];
        UIScreenMetaData& screen_metas = ui_manager.screenMetaData[screen];

        if (ImGui::Button("Save Entities to File"))
        {
            std::vector<ECS::EntityMetaData*> meta_data;

            screen_metas.clear();

            for (u32 i = 0; i < entities.size(); i++)
            {
                screen_metas.push_back(ECS::EntityMetaData());
                ECS::EntityMetaData& md = screen_metas.back();

                SerialiseEntity(entities[i], md);
            }

            BasicString bs = FileManager::Get()->folderPath(FileManager::SaveData);

            char buffer[512]{ 0 };
            snprintf(buffer, 512, "%s\\%s.uiscreen", bs.c_str(), screen);

            SaveMetaDataToJson(buffer, screen_metas);
        }

        ImGui::SameLine();
        if (ImGui::Button("Load Entities from File") || force_load_entities_from_file)
        {
            for (u32 i = 0; i < entities.size(); i++)
            {
                ecs->entities.KillEntity(entities[i]);
            }
            entities.clear();
            screen_metas.clear();

            BasicString bs = FileManager::Get()->folderPath(FileManager::SaveData);

            char buffer[512]{ 0 };
            snprintf(buffer, 512, "%s\\%s.uiscreen", bs.c_str(), screen);

            LoadMetaDataFromJson(buffer, screen_metas);

            ui_manager.CloseScreen(screen);
            ui_manager.OpenScreen(screen);
        }

        UIEntityBuilder();

        Entity selected_entity = SelectableUIEntityList();

        Entity entity_on_cursor = GetEntityAtCursor(selected_entity);
        if (entity_on_cursor != EntityInvalid)
            selected_entity = entity_on_cursor;

        DebugMenu::SelectEntity(selected_entity);

        // while we dont have anything selected click to select something
        if (!ecs->IsAlive(selected_entity))
        {
            //selected_entity = GetEntityAtCursor();
            //DebugMenu::SelectEntity(selected_entity);
        }
        // handle controls for positioning/sizing etc.
        else
        {
            DoEntityControlPanel(selected_entity, screen_metas, entities);
        }

        if (im->isCursorPressed(Cursor::Right))
        {
            selected_entity = ECS::EntityInvalid;
            DebugMenu::SelectEntity(selected_entity);
        }
        if (im->isPressed(Button::Delete))
        {
            Erase(entities, selected_entity);
            ecs->entities.KillEntity(selected_entity);
        }

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
        AddTestScreen("debug_screen");
    }

    void Close()
    {
        DebugMenu::ToggleUIWindow(false);
    }
}