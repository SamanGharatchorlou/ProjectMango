#include "pch.h"

#include "imgui-master/imgui.h"
#include "ECS/Components/IncludeComponents.h"
#include "Game/Camera/Camera.h"
#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiMenu.h"
#include "Graphics/RenderManager.h"

namespace DebugMenu
{
    struct TweakerState
    {
        bool sendRenderLayerInfo = false;
        std::vector<int> renderPacks;
    };

    static TweakerState s_state;

    void DoTweakerWindow()
    {
        ImGui::Begin("Entity Window", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        SharedState& state = GetSharedState();

        ECS::Entity entity = ECS::Faction::GetPlayer();

        if (ECS::Health* health = GetComponent(Health, entity))
        {
            ImGui::Checkbox("Player Invulnerable", &health->invulnerable);
        }

        if (ImGui::Button("Kill Enemy"))
        {
            ECS::Entity enemy = ECS::Faction::GetEnemy();
            if (ECS::Health* health = GetComponent(Health, enemy))
            {
                health->ApplyDamage(FLT_MAX);
            }
        }

        ImGui::Checkbox("Can buy any card", &state.canBuyAnyCard);

        ImGui::Checkbox("Display Camera bits", &state.displayCameraInfo);
        if (state.displayCameraInfo)
        {
            Camera* cam = Camera::Get();
            RectF rect = cam->GetRect();
            DebugDraw::RectOutline(rect, SColour::Green);
            DebugDraw::Point(rect.Center(), SColour::Green);

            const ECS::Transform& transform = GetComponentRef(Transform, cam->targetEntity);
            DebugDraw::Point(transform.GetObjectCenter(), SColour::Red);
        }

        s_state.sendRenderLayerInfo = false;
        if (ImGui::TreeNode("Render Layers"))
        {
            s_state.sendRenderLayerInfo = true;

            RenderManager* rm = RenderManager::Get();
            for (u32 i = 0; i < s_state.renderPacks.size(); i++)
            {
                ImGui::Text("%d. %d", i, s_state.renderPacks[i]);
            }

            ImGui::TreePop();
        }

        ImGui::Checkbox("Turn Logging", &state.turnLogActive);
        if (state.turnLogActive)
        {
            if (ImGui::TreeNode("Turn Log"))
            {
                for (u32 i = 0; i < state.turnLog.size(); i++)
                {
                    ImGui::Text(state.turnLog[i].c_str());
                }

                ImGui::TreePop();
            }
        }
        else
        {
            state.turnLog.clear();
        }

        ImGui::End();
    }

    void SendRenderLayerInfo(const std::vector<RenderPack>* render_packs)
    {
        if (!s_state.sendRenderLayerInfo)
            return;

        s_state.renderPacks.clear();

        for (u32 i = 0; i < c_RenderLayers; i++)
        {
            const std::vector<RenderPack>& layer = render_packs[i];

            s_state.renderPacks.push_back((int)layer.size());
        }
    }
}