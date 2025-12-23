#include "pch.h"
#include "ImGuiMainWindows.h"

#include "Core/Helpers.h"
#include "ImGuiHelpers.h"
#include "imgui-master/imgui.h"

#include "Debugging/ImGui/Components/ComponentDebugMenu.h"
#include "ECS/Components/AIComponents.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Biome.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/GameComponents.h"
#include "ECS/Components/UIComponents.h"
#include "ECS/Components/Physics.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/EntityManager.h"
#include "Game/Camera/Camera.h"
#include "Game/FrameRateController.h"
#include "Game/States/GameState.h"
#include "Game/SystemStateManager.h"
#include "Graphics/RenderManager.h"
#include "Input/InputManager.h"
#include "System/Files/ConfigManager.h"
#include "Audio/AudioManager.h"
#include "Game/Camera/Camera.h"

using namespace DebugMenu;

static TweakerState s_state;

TweakerState& DebugMenu::GetState()
{
    return s_state;
}

static ECS::Entity s_selectedEntity = 0;
static StringBuffer64 filterBuffer;

u32 DebugMenu::GetSelectedEntity() { return s_selectedEntity; }
void DebugMenu::SelectEntity(ECS::Entity entity) { s_selectedEntity = entity; }

static int id_numb = 0;

#define DoRemoveButton(type) \
    ImGui::PushID(id_numb++); do_dropdown = true; \
    if(ImGui::Button("-")) {\
        RemoveComponent(type, s_selectedEntity); do_dropdown = false; }\
    if(ImGui::IsItemHovered()) \
        ImGui::SetTooltip("%s", ECS::ComponentNames[(int)ECS::Component::type]); \
    ImGui::SameLine(); ImGui::PopID(); \

#define ComponentDropdown(menu) \
    if(do_dropdown) \
        SetFlag<u64>(type, ECS::archetypeBit((ECS::Component::Type)menu(s_selectedEntity)));

#define DoComponentDropdown(component) \
    if(HasComponent(component,s_selectedEntity)) {\
        DoRemoveButton(component); ComponentDropdown(Do##component##DebugMenu); }

// Entity Window
void DebugMenu::DoEntitySystemWindow()
{
    ImGui::Begin("Entity Window", nullptr, ImGuiWindowFlags_MenuBar);
    
    ECS::EntityManager& em = ecs->entities;

    ImGui::Text("Selected Entity: %d", (int)s_selectedEntity);
    ImGui::InputText("Entity Filter", filterBuffer.buffer(), filterBuffer.bufferLength());

    bool is_number = filterBuffer.length() > 0;
    for( u32 i = 0; i < filterBuffer.length(); i++ )
    {
        char* c = filterBuffer.buffer() + i;
        int value = *c;
        if(!std::isdigit(value))
        {
            is_number = false;
            break;
        }
    }

    if(is_number)
    {
        int number = std::atoi(filterBuffer.c_str());
        if(ecs->IsAlive(number))
        {
            s_selectedEntity = number;
        }
        else
        {
        }
    }

    if(!ecs->IsAlive(s_selectedEntity))
    {
        ImGui::Text("Entity %d is dead", s_selectedEntity);
    }

    const char* selected = ECS::GetName(s_selectedEntity);
    if (!selected)
        selected = "";

    static bool s_ignoreTerrain = true;
    ImGui::Checkbox("Ignore Terrain Entities", &s_ignoreTerrain);

    if (ImGui::BeginCombo("Entities", selected, 0))
    {
        const ECS::ComponentArray<ECS::EntityData>& entity_data = GetAllComponents(EntityData);
        for (auto iter = entity_data.entityToComponent.begin(); iter != entity_data.entityToComponent.end(); iter++)
        {
            const  ECS::EntityData& ed = entity_data.GetComponentByIndex(iter->second);
            StringBuffer64 entity_name = StringBuffer64(ed.id.c_str()).to_lower();

            if (filterBuffer.length() > 0 && !is_number)
            {
                StringBuffer64 filter = filterBuffer.to_lower();
                const char* value = strstr( entity_name.c_str(), filter.c_str() );
                if ( !value )
                    continue;
            }

            if(s_ignoreTerrain && IsTerrain(ed.entity))
            {
                continue;
            }

            ImGui::PushID(iter->first);

            const bool is_selected = iter->first == s_selectedEntity;
            if (ImGui::Selectable(entity_name.c_str(), is_selected))
                s_selectedEntity = iter->first;

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();

            ImGui::PopID();
        }

        ImGui::EndCombo();
    }

    if (ecs->IsAlive(s_selectedEntity))
    {		    
        if(const ECS::Transform* transform = GetComponent(Transform, s_selectedEntity))
        {      
		    RectF rect(transform->worldPosition, transform->size);
		    DebugDraw::RectOutline(rect, SColour::Blue);
        }

		ECS::Health* health = GetComponent(Health, s_selectedEntity);
        if(ImGui::ActiveButton("Kill Entity", health != nullptr))
        {
            health->currentHealth = 0;
        }

        if(ImGui::Button("Destroy Entity"))
        {
            em.KillEntity(s_selectedEntity);
            ImGui::End();
            return;
        }

        id_numb = 0;
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
        DoComponentDropdown(PlayerController);
        DoComponentDropdown(Health);
        DoComponentDropdown(Biome);
        DoComponentDropdown(UIButton);
        DoComponentDropdown(UIText);
        DoComponentDropdown(CoinStack);
        DoComponentDropdown(Inventory);
        DoComponentDropdown(Card);
        DoComponentDropdown(BehaviourState);
        DoComponentDropdown(EntityState);

        ECS::Archetype entity_type = em.GetAchetype(s_selectedEntity);
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
	        const int frame_count = frc.FrameCount();

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

static bool s_displayStatics = false;
static bool s_displayDynamics = false;
static bool s_displayTransforms = true;
static bool s_displayObjectCenter = true;
static bool s_displayObjectRect = true;
static DebugDrawType s_drawType = DebugDrawType::RectOutline;


void DebugMenu::DoTransformWindow() 
{
    const ECS::ComponentArray<ECS::Collider>& colliders = GetAllComponents(Collider);
	const u32 count = (u32)colliders.entityToComponent.size();

    ImGui::Checkbox("Display Statics", &s_displayStatics);
    ImGui::Checkbox("Display Dynamics", &s_displayDynamics);
    ImGui::Checkbox("Display Raycasts", &s_state.drawRaycasts);
    ImGui::Checkbox("Display Transform Rect", &s_displayTransforms);
    ImGui::Checkbox("Display Object Rect", &s_displayObjectRect);
    ImGui::Checkbox("Display Object Center", &s_displayObjectCenter);

    ImGui::DoDebugRenderTypeDropDown(s_drawType);

    for( auto iter = colliders.entityToComponent.begin(); iter != colliders.entityToComponent.end(); iter++ )
	{
		u32 component_index = iter->second;
		const ECS::Collider& collider = colliders.GetComponentByIndex(component_index);

		// ignore static colliders, they dont move
        bool is_static = collider.HasFlag(ECS::Collider::Static);
		if(!s_displayStatics && is_static)
            continue;
        if(!s_displayDynamics && !is_static)
            continue;

        DrawCollider(collider);
	}
    
    if(s_displayTransforms)
    {
        const ECS::ComponentArray<ECS::Transform>& transforms = GetAllComponents(Transform);
	    const u32 count = (u32)transforms.entityToComponent.size();

        for( auto iter = transforms.entityToComponent.begin(); iter != transforms.entityToComponent.end(); iter++ )
	    {
		    u32 component_index = iter->second;
		    const ECS::Transform& transform = transforms.GetComponentByIndex(component_index);
            
			RectF rect(transform.worldPosition, transform.size);
			DebugDraw::RectOutline(rect, SColour::Green);
	    }
    }

    if(s_displayObjectRect)
    {
        const ECS::ComponentArray<ECS::Transform>& transforms = GetAllComponents(Transform);
	    const u32 count = (u32)transforms.entityToComponent.size();

        for( auto iter = transforms.entityToComponent.begin(); iter != transforms.entityToComponent.end(); iter++ )
	    {
		    u32 component_index = iter->second;
		    const ECS::Transform& transform = transforms.GetComponentByIndex(component_index);
            
			DebugDraw::RectOutline(transform.GetObjectRect(), SColour::Blue);
	    }
    }

    if(s_displayObjectCenter)
    {
         const ECS::ComponentArray<ECS::Transform>& transforms = GetAllComponents(Transform);
	    const u32 count = (u32)transforms.entityToComponent.size();

        for( auto iter = transforms.entityToComponent.begin(); iter != transforms.entityToComponent.end(); iter++ )
	    {
		    u32 component_index = iter->second;
		    const ECS::Transform& transform = transforms.GetComponentByIndex(component_index);
     
            VectorF object_center = transform.GetObjectCenter();
			DebugDraw::Point(object_center, SColour::Green);
	    }
    }
}

DebugMenu::GamePlayerState s_gamePlayerState;

DebugMenu::GamePlayerState& DebugMenu::GetGamePlayerState()
{
    return s_gamePlayerState;
}

static bool s_gamePlayer = false;
static bool s_nextFrame = false;
static bool s_selectingShakeSource = false;

struct FrameData
{
    TimerF update_timer;

    float frameRate = 0.0f;
    float gameTime = 0.0f;
    float realTime = 0.0f;
    float waitTime = 0.0f;
    float waitPercentage = 0.0f;
};

static FrameData s_frameData;

void DebugMenu::DoGameStateWindow() 
{
    if(ImGui::Button("Restart Game State"))
    {
        GameData::Get().systemStateManager->mStates.replaceState(new GameState);
    }

    if(ImGui::Button("Reload Configs"))
    {
        GameData::Get().configs->mConfigs.clear();
    }

    ImGui::Checkbox("Game Player", &s_gamePlayerState.isActive);

    if(ImGui::Button("Next Frame"))
    {
        s_gamePlayerState.nextFrame = true;
    }
    
    if (ImGui::TreeNode("Frame Info"))
    {
        FrameRateController& fc =  FrameRateController::Get();
        ImGui::Text("max frme rate: %d", fc.frameRateCap);
        if(ImGui::Button("Update Framerate Cap"))
        {
            GameData::Get().configs->mConfigs.clear();

	        ConfigManager* cm = ConfigManager::Get();
            
            fc.start();

            s_frameData = FrameData();
        } 

        if(!s_frameData.update_timer.IsRunning())
            s_frameData.update_timer.Start();

        if(s_frameData.update_timer.GetSeconds() > 1.0f)
        {
            s_frameData.update_timer.Restart();
            s_frameData.frameRate = fc.FrameCount() / fc.gameTimer.GetSeconds();
            s_frameData.gameTime = fc.frameTimer.GetMilliseconds();

            if(fc.capTimer.IsRunning())
            {
                s_frameData.realTime = fc.capTimer.GetMilliseconds();
            
                float wait_time = (1000.0f / fc.frameRateCap) - fc.capTimer.GetMilliseconds();
                float percentage = wait_time / (1000.0f / fc.frameRateCap);
                s_frameData.waitTime = wait_time;
                s_frameData.waitPercentage = percentage;
            }
        }

        ImGui::Text( "Framerate(fps): %.f", s_frameData.frameRate );
        ImGui::Text( "Game frame time(ms): %.f", s_frameData.gameTime);
        
        if(fc.capTimer.IsRunning())
        {
            ImGui::Text( "Real frame time(ms): %.f", s_frameData.realTime );
            ImGui::Text( "Frame wait time(ms): %.f (%.f)", s_frameData.waitTime, s_frameData.waitPercentage );
        }

        ImGui::TreePop();
    }
    
    
    if (ImGui::TreeNode("Cursore Info"))
    {
	    InputManager* input = InputManager::Get();
        VectorF cursor_pos = input->cursorWorldPosition();

        ImGui::Text( "Cursor sceen pos: %f, %f", input->cursorScreenPosition().x, input->cursorScreenPosition().y );
        ImGui::Text( "Cursor world pos: %f, %f", input->cursorWorldPosition().x, input->cursorWorldPosition().y );

        ECS::ComponentArray<ECS::Collider>& colliders = GetAllComponents(Collider);

        for (auto iter = colliders.entityToComponent.begin(); iter != colliders.entityToComponent.end(); iter++)
        {
            ECS::Collider& collider = colliders.GetComponentByIndex(iter->second);
            if(collider.Contains(cursor_pos))
            {
                ImGui::Text("Cursor hit: %s(%d)", ECS::GetName(collider.entity), collider.entity);
            }
        }
    
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Audio"))
    {
		AudioManager* am = AudioManager::Get();

        for( u32 i = 0; i < c_mixerChannels; i++ )
        {
            ImGui::PushID(i);
            const Channel& channel = am->mSoundController.channels[i];
            const char* id = "";
            if(channel.sound)
            {
                if(am->mSoundController.IsPlaying(channel))
                    id = am->GetSoundEffectId(channel.sound);
            }

            ImGui::Text("%d. %s", i, id);
            ImGui::PopID();
        }
    
        ImGui::TreePop();
    }

    if( ImGui::TreeNode("Camera"))
    {
        CameraShake& shakey = Camera::Get()->shakeyCam;
        ImGui::InputFloat("speed", &shakey.speed);

        float trauma[2] { shakey.maxTrauma.x, shakey.maxTrauma.y };
        if(ImGui::InputFloat2("magnitude", trauma ))
        {
            shakey.maxTrauma.x = trauma[0];
            shakey.maxTrauma.y = trauma[1];
        }

        if(ImGui::Button("Shake Camera"))
        {
			Camera::Get()->AddShake(10);
        }

        const char* text = s_selectingShakeSource ? "Select source" : "Shake camera from next click source";
        if(ImGui::Button(text))
        {
            s_selectingShakeSource = !s_selectingShakeSource;
        }
        
        if(s_selectingShakeSource)
        {
            InputManager* im = GameData::Get().inputManager;
            if(im->isCursorPressed(Cursor::Left))
            {
			    Camera::Get()->AddShake(10, im->cursorWorldPosition());
            }
        }

        ImGui::Text("progress: %f%", shakey.x * 100.0f);

        ImGui::TreePop();
    }

}

static bool s_debugCamera = false;

static bool s_getRenderLayerData = false;
std::vector<int> s_renderPacks;



void DebugMenu::DoTweakerWindow() 
{
	ECS::Entity entity = Target::GetPlayer();

    if(ECS::Health* health = GetComponent(Health, entity))
    {
        ImGui::Checkbox("Player Invulnerable", &health->invulnerable);
    }

    ImGui::Checkbox("Can buy any card", &s_state.canBuyAnyCard);

    ImGui::Checkbox("Display Camera bits", &s_debugCamera);
    if (s_debugCamera)
    {
        Camera* cam = Camera::Get();
        RectF rect = cam->GetRect();
        DebugDraw::RectOutline(rect, SColour::Green);
        DebugDraw::Point(rect.Center(), SColour::Green);

        const ECS::Transform& transform = GetComponentRef(Transform, cam->targetEntity);
        DebugDraw::Point(transform.GetObjectCenter(),SColour::Red);
    }

    s_getRenderLayerData = false;
    if (ImGui::TreeNode("Render Layers"))
	{
        s_getRenderLayerData = true; 

        RenderManager* rm = RenderManager::Get();
        for( u32 i = 0; i < s_renderPacks.size(); i++ )
        {
            ImGui::Text("%d. %d", i, s_renderPacks[i]);
        }

        ImGui::TreePop();
    }
}


void DebugMenu::SendRenderLayerInfo(const std::vector<RenderPack>* render_packs)
{
    if(!s_getRenderLayerData)
        return;

    s_renderPacks.clear();

    for( u32 i = 0; i < c_RenderLayers; i++ )
    {
        const std::vector<RenderPack>& layer = render_packs[i];

        s_renderPacks.push_back((int)layer.size());
    }
}