#include "pch.h"

#include "imgui-master/imgui.h"
#include "ECS/Components/IncludeComponents.h"
#include "Graphics/RenderManager.h"
#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/Components/ComponentDebugMenu.h"

namespace DebugMenu
{
    struct TransformState
    {
        bool displayStatics = false;
        bool displayDynamics = false;
        bool displayTransforms = true;
        bool displayObjectCenter = true;
        bool displayObjectRect = true;
        DebugRender::DrawType drawType = DebugRender::DrawType::RectOutline;
    };

    static TransformState s_state;

    void DoTransformWindow()
    {
        using namespace ECS;

        const ECS::ComponentArray<ECS::Collider>& colliders = GetAllComponents(Collider);
        const u32 count = (u32)colliders.entityToComponent.size();

        ImGui::Checkbox("Display Statics", &s_state.displayStatics);
        ImGui::Checkbox("Display Dynamics", &s_state.displayDynamics);
        //ImGui::Checkbox("Display Raycasts", &s_state.drawRaycasts);
        ImGui::Checkbox("Display Transform Rect", &s_state.displayTransforms);
        ImGui::Checkbox("Display Object Rect", &s_state.displayObjectRect);
        ImGui::Checkbox("Display Object Center", &s_state.displayObjectCenter);

        const char* arrayDebugDrawType[4] = { "Point", "Line", "RectOutline", "RectFill" };
        ImGui::Combo("Draw Types", (int*)&s_state.drawType, arrayDebugDrawType, 4);

        for (auto iter = colliders.entityToComponent.begin(); iter != colliders.entityToComponent.end(); iter++)
        {
            u32 component_index = iter->second;
            const ECS::Collider& collider = colliders.GetComponentByIndex(component_index);

            // ignore static colliders, they dont move
            bool is_static = collider.HasFlag(ECS::Collider::Static);
            if (!s_state.displayStatics && is_static)
                continue;
            if (!s_state.displayDynamics && !is_static)
                continue;

            DrawCollider(collider);
        }

        if (s_state.displayTransforms)
        {
            const ECS::ComponentArray<ECS::Transform>& transforms = GetAllComponents(Transform);
            const u32 count = (u32)transforms.entityToComponent.size();

            for (auto iter = transforms.entityToComponent.begin(); iter != transforms.entityToComponent.end(); iter++)
            {
                u32 component_index = iter->second;
                const ECS::Transform& transform = transforms.GetComponentByIndex(component_index);

                if (s_state.displayTransforms)
                {
                    RectF rect(transform.worldPosition, transform.size);
                    DebugDraw::RectOutline(rect, SColour::Green);
                }
                if (s_state.displayObjectRect)
                {
                    RectF object_rect = transform.GetObjectRect();
                    DebugDraw::RectOutline(object_rect, SColour::Blue);
                }
                if (s_state.displayObjectCenter)
                {
                    VectorF object_center = transform.GetObjectCenter();
                    DebugDraw::Point(object_center, SColour::Green);
                }
            }
        }
    }
}