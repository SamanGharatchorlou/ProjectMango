#include "pch.h"

#include "imgui-master/imgui.h"
#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "ImGuiMainWindows.h"
#include "Game/States/GameState.h"
#include "Game/SystemStateManager.h"
#include "Game/FrameRateController.h"
#include "Input/InputManager.h"
#include "Audio/AudioManager.h"
#include "Game/Camera/Camera.h"

namespace DebugMenu
{
    using namespace ECS;

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
    static bool s_selectingShakeSource = false;

    void DoGameStateWindow()
    {
        if (ImGui::Button("Restart Game State"))
        {
            GameData::Get().systemStateManager->mStates.replaceState(new GameState);
        }

        if (ImGui::Button("Reload Configs"))
        {
            GameData::Get().configs->mConfigs.clear();
        }

        ImGui::Checkbox("Game Player", &GetSharedState().isActive);

        if (ImGui::Button("Next Frame"))
        {
            GetSharedState().nextFrame = true;
        }

        if (ImGui::TreeNode("Frame Info"))
        {
            FrameRateController& fc = FrameRateController::Get();
            ImGui::Text("max frme rate: %d", fc.frameRateCap);
            if (ImGui::Button("Update Framerate Cap"))
            {
                GameData::Get().configs->mConfigs.clear();

                ConfigManager* cm = ConfigManager::Get();

                fc.start();

                s_frameData = FrameData();
            }

            if (!s_frameData.update_timer.IsRunning())
                s_frameData.update_timer.Start();

            if (s_frameData.update_timer.GetSeconds() > 1.0f)
            {
                s_frameData.update_timer.Restart();
                s_frameData.frameRate = fc.FrameCount() / fc.gameTimer.GetSeconds();
                s_frameData.gameTime = fc.frameTimer.GetMilliseconds();

                if (fc.capTimer.IsRunning())
                {
                    s_frameData.realTime = fc.capTimer.GetMilliseconds();

                    float wait_time = (1000.0f / fc.frameRateCap) - fc.capTimer.GetMilliseconds();
                    float percentage = wait_time / (1000.0f / fc.frameRateCap);
                    s_frameData.waitTime = wait_time;
                    s_frameData.waitPercentage = percentage;
                }
            }

            ImGui::Text("Framerate(fps): %.f", s_frameData.frameRate);
            ImGui::Text("Game frame time(ms): %.f", s_frameData.gameTime);

            if (fc.capTimer.IsRunning())
            {
                ImGui::Text("Real frame time(ms): %.f", s_frameData.realTime);
                ImGui::Text("Frame wait time(ms): %.f (%.f)", s_frameData.waitTime, s_frameData.waitPercentage);
            }

            ImGui::TreePop();
        }


        if (ImGui::TreeNode("Cursore Info"))
        {
            InputManager* input = InputManager::Get();
            VectorF cursor_pos = input->cursorWorldPosition();

            ImGui::Text("Cursor sceen pos: %f, %f", input->cursorScreenPosition().x, input->cursorScreenPosition().y);
            ImGui::Text("Cursor world pos: %f, %f", input->cursorWorldPosition().x, input->cursorWorldPosition().y);

            ECS::ComponentArray<ECS::Collider>& colliders = GetAllComponents(Collider);

            for (auto iter = colliders.entityToComponent.begin(); iter != colliders.entityToComponent.end(); iter++)
            {
                ECS::Collider& collider = colliders.GetComponentByIndex(iter->second);
                if (collider.Contains(cursor_pos))
                {
                    ImGui::Text("Cursor hit: %s(%d)", ECS::GetName(collider.entity), collider.entity);
                }
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Audio"))
        {
            AudioManager* am = AudioManager::Get();

            for (u32 i = 0; i < c_mixerChannels; i++)
            {
                ImGui::PushID(i);
                const Channel& channel = am->mSoundController.channels[i];
                const char* id = "";
                if (channel.sound)
                {
                    if (am->mSoundController.IsPlaying(channel))
                        id = am->GetSoundEffectId(channel.sound);
                }

                ImGui::Text("%d. %s", i, id);
                ImGui::PopID();
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Camera"))
        {
            CameraShake& shakey = Camera::Get()->shakeyCam;
            ImGui::InputFloat("speed", &shakey.speed);

            float trauma[2]{ shakey.maxTrauma.x, shakey.maxTrauma.y };
            if (ImGui::InputFloat2("magnitude", trauma))
            {
                shakey.maxTrauma.x = trauma[0];
                shakey.maxTrauma.y = trauma[1];
            }

            if (ImGui::Button("Shake Camera"))
            {
                Camera::Get()->AddShake(10);
            }

            const char* text = s_selectingShakeSource ? "Select source" : "Shake camera from next click source";
            if (ImGui::Button(text))
            {
                s_selectingShakeSource = !s_selectingShakeSource;
            }

            if (s_selectingShakeSource)
            {
                InputManager* im = GameData::Get().inputManager;
                if (im->isCursorPressed(Cursor::Left))
                {
                    Camera::Get()->AddShake(10, im->cursorWorldPosition());
                }
            }

            ImGui::Text("progress: %f%", shakey.x * 100.0f);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Turn Order"))
        {
            if (GameState* game_state = GameState::GetActive())
            {
                ImGui::Text("Game State turn index: %d", game_state->turnIndex);
            }

            std::vector<Entity> turn_order;
            ComponentArray<TurnState>& turn_states = GetAllComponents(TurnState);
            for (auto iter = turn_states.entityToComponent.begin(); iter != turn_states.entityToComponent.end(); iter++)
            {
                turn_order.push_back(iter->first);
            }

            std::sort(turn_order.begin(), turn_order.end(), [](Entity a, Entity b) {
                TurnState& turn_A = GetComponentRef(TurnState, a);
                TurnState& turn_B = GetComponentRef(TurnState, b);
                return turn_A.initiative < turn_B.initiative;
                });

            TurnState* active_state = TurnState::GetActive();

            for (u32 i = 0; i < turn_order.size(); i++)
            {
                TurnState& turn = GetComponentRef(TurnState, turn_order[i]);

                if (active_state && active_state->entity == turn.entity)
                    ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Turn(%d): %s", turn.initiative, GetName(turn_order[i]));
                else
                    ImGui::Text("Turn(%d): %s", turn.initiative, GetName(turn_order[i]));
            }

            ImGui::TreePop();
        }
    }

}