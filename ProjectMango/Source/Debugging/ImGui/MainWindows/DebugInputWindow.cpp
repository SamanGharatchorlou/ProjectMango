#include "pch.h"

#include "imgui-master/imgui.h"
#include "Input/InputManager.h"
#include "Game/FrameRateController.h"

namespace DebugMenu
{
    void DoInputWindow()
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
}