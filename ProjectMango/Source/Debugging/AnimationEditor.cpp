#include "pch.h"
#include "AnimationEditor.h"

#include "ImGui/ImGuiHelpers.h"
#include "Graphics/TextureManager.h"
#include "Graphics/Texture.h"
#include "System/Window.h"
#include "Graphics/RenderManager.h"
#include "Input/InputManager.h"
#include "Game/FrameRateController.h"
#include "ECS/Components/Animator.h"
#include "Animations/AnimationReader.h"
#include "Animations/CharacterStates.h"
#include "ECS/Components/Components.h"
#include "ECS/EntSystems/AnimationSystem.h"

#include "Core/Helpers.h"
#include "imgui.h"

namespace AnimationEditor
{
    struct AnimationState
	{
		StringBuffer64 selectedSpriteSheet;
		VectorI frameCounts = VectorI(17,14);

		std::vector<VectorF> selectedFrames;
		std::vector<VectorI> selectedFrameIndexes;
        
        struct Config
        {
            StringBuffer64 selected;
            ECS::Animator animator;

            TimeState state;
            int pausedFrame;
        };

        struct CursorSelection
        {
            VectorF topLeft;
            VectorF botRight;

            RectF selectionRect;
        };

        CursorSelection cursorSelection;

        Config configAnim;

		int targetFrame = 0;
		float frameTime = 0.1f;
		float frameTimer = 0.0f;
		bool isPlayingFrames = true;
	};

    static AnimationState s_state;

	void DoEditor()
	{
		ImGui::Begin("Animation Editor", nullptr, ImGuiWindowFlags_MenuBar);
                
        FrameRateController& fc = FrameRateController::Get();
	    RenderManager* rm = GameData::Get().renderManager;
        InputManager* im = GameData::Get().inputManager;
	    const VectorF window_size = GameData::Get().window->size();
        const VectorF y_spacing = window_size * VectorF(0.0f, 0.1f);

        VectorF draw_point_TL = y_spacing;
        
        if( ImGui::TreeNode("Show Frame Split") )
        {
            // file select dropdown
            ECS::EntityCoordinator* ecs = GameData::Get().ecs;
            if (ImGui::BeginCombo("Sprite Sheet", s_state.selectedSpriteSheet.c_str()))
            {
                FileManager* fm = FileManager::Get();
                std::vector<BasicString> file_names = fm->fileNamesInFolder(FileManager::Image_Animations);

                for( u32 i = 0; i < file_names.size(); i++ )
                {
                    const bool is_selected =  StringCompare(s_state.selectedSpriteSheet.c_str(), file_names[i].c_str());

                    if (ImGui::Selectable(file_names[i].c_str(), is_selected))
                        s_state.selectedSpriteSheet = file_names[i].c_str();

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            // animation editing
            if( strlen(s_state.selectedSpriteSheet.c_str()) > 0 )
            {
                if(const Texture* selected_tx = TextureManager::Get()->getTexture( s_state.selectedSpriteSheet.c_str(), FileManager::Image_Animations ))
		        {
			        VectorF dim = selected_tx->originalDimentions;
			        VectorF texture_size(window_size.x, (window_size.x * dim.y) / dim. x);

			        RectF animation(draw_point_TL, texture_size);
                    draw_point_TL += VectorF(0, texture_size.y) + y_spacing;

			        RenderPack pack(selected_tx, animation, 1);
			        rm->AddRenderPacket(pack);

			        DebugDraw::RectOutline(animation, Colour::Yellow);

                    ImGui::VectorText("Texture Size", texture_size);
                    ImGui::InputVectorI("Frame Size", s_state.frameCounts);

                    int frame_split_x = std::max(1, s_state.frameCounts.x);
                    int frame_split_y = std::max(1, s_state.frameCounts.y);
                    const VectorF frame_size = animation.Size() / VectorI(frame_split_x,frame_split_y).toFloat();
                    
                    for( u32 ix = 0; ix < frame_split_x; ix++ )
                    {
                        VectorF pointA((float)ix * frame_size.x, 0.0f);
                        VectorF pointB((float)ix * frame_size.x, texture_size.y);
                        DebugDraw::Line(pointA + animation.TopLeft(), pointB + animation.TopLeft(), Colour::Blue);
                    }
                    for( u32 iy = 0; iy < frame_split_y; iy++ )
                    {
                        VectorF pointA(0.0f, (float)iy * frame_size.y);
                        VectorF pointB(texture_size.x, (float)iy * frame_size.y);
                        DebugDraw::Line(pointA + animation.TopLeft(), pointB + animation.TopLeft(), Colour::Blue);
                    }

                    if(im->isCursorHeld(Cursor::ButtonType::Left))
                    {
                        const VectorF cursor_pos = im->cursorPosition();

                        for( u32 ix = 0; ix < frame_split_x; ix++ )
                        {   
                            for( u32 iy = 0; iy < frame_split_y; iy++ )
                            {
                                VectorF pos = frame_size * VectorI(ix,iy).toFloat() + animation.TopLeft();
                                RectF rect(pos, frame_size);
                                
                                if(PointInRect(rect, cursor_pos))
                                {                               
                                    if( !Contains(s_state.selectedFrames, pos) )
                                    {
                                        s_state.selectedFrames.push_back(pos);
                                        s_state.selectedFrameIndexes.push_back(VectorI(ix,iy));
                                    }
                                }
                            }
                        }
                    }

                    if(im->isCursorPressed(Cursor::ButtonType::Right))
                    {
                        s_state.selectedFrames.clear();
                        s_state.selectedFrameIndexes.clear();
                        s_state.targetFrame = 0;
                    }

                    for( u32 f = 0; f < s_state.selectedFrames.size(); f++ )
                    {
                        VectorF pos = s_state.selectedFrames[f];
                        DebugDraw::RectOutline(RectF(pos, frame_size), Colour::Purple);
                    }
                    
                    ImGui::Text("Selected Frame count: %d", s_state.selectedFrames.size());
                    ImGui::Text("Active Frame: %d", s_state.targetFrame);

                    if(s_state.selectedFrames.size() > 0)
                    {
                        ImGui::PushID("sprite sheet selector");

                        // Display selected frames
                        const VectorF real_frame_size = dim / VectorI(frame_split_x,frame_split_y).toFloat();

                        VectorF frame_texture_size(window_size.x, (window_size.x * real_frame_size.y) / real_frame_size.x);
                        VectorF adjusted_frame_texture_size = frame_texture_size;
                        adjusted_frame_texture_size.y /= s_state.selectedFrames.size();

                        VectorI top_left = s_state.selectedFrameIndexes.front();
                        VectorI bot_right = s_state.selectedFrameIndexes.back();
                        VectorF top_left_pos = real_frame_size * top_left.toFloat();
                        VectorF size = ((bot_right - top_left) + VectorI(1,1)).toFloat() * real_frame_size;

                        RectF subRect(top_left_pos, size);

                        RectF renderRect( draw_point_TL, adjusted_frame_texture_size );
                        draw_point_TL += VectorF(0, adjusted_frame_texture_size.y) + y_spacing;

			            RenderPack pack(selected_tx, renderRect, 1);
                        pack.subRect = subRect;
			            rm->AddRenderPacket(pack);

                        DebugDraw::RectOutline(renderRect, Colour::Yellow);

                        // display the frame on the row showing the active render frame
                        VectorF render_row_frame_size = VectorF(renderRect.Width() / (float)s_state.selectedFrames.size(), renderRect.Height());
                        VectorF renderRowFrame_topLeft = renderRect.TopLeft() + VectorF(s_state.targetFrame * render_row_frame_size.x, 0.f);
                        RectF renderRowFrameRect(renderRowFrame_topLeft, render_row_frame_size);
                        DebugDraw::RectOutline(renderRowFrameRect, Colour::Blue);

                        // Display Target Frame
                        ImGui::DragFloat("frame time", &s_state.frameTime);

                        if (ImGui::Button("<- Previous Frame"))
					    {
                            s_state.targetFrame--;
                            if(s_state.targetFrame < 0)
                                s_state.targetFrame = (int)s_state.selectedFrameIndexes.size() - 1;

                            s_state.isPlayingFrames = false;
					    }
                        	
                        ImGui::SameLine();	
                        ImGui::Checkbox("Play Frames", &s_state.isPlayingFrames);	

                        ImGui::SameLine();
					    if (ImGui::Button("Next Frame ->"))
					    {
                            s_state.targetFrame++;
                            s_state.targetFrame = s_state.targetFrame % s_state.selectedFrameIndexes.size();

                            s_state.isPlayingFrames = false;
					    }

                        RectF renderFrameRect(draw_point_TL, frame_texture_size);
                        draw_point_TL += VectorF(0, frame_texture_size.y) + y_spacing;

                        VectorI frame_top_left_index = s_state.selectedFrameIndexes[s_state.targetFrame];
                        VectorF frame_top_left_pos = real_frame_size * frame_top_left_index.toFloat(); 
                        RectF frameSubRect(frame_top_left_pos, real_frame_size);
                            			            
                        RenderPack frame_pack(selected_tx, renderFrameRect, 1);
                        frame_pack.subRect = frameSubRect;
			            rm->AddRenderPacket(frame_pack);

                        DebugDraw::RectOutline(renderFrameRect, Colour::Yellow);

                        if(s_state.isPlayingFrames)
                        {
                            s_state.frameTimer += fc.delta();

                            if(s_state.frameTimer > s_state.frameTime)
                            {
                                s_state.targetFrame++;
                                s_state.targetFrame = s_state.targetFrame % s_state.selectedFrameIndexes.size();

                                s_state.frameTimer = 0.0f;
                            }
                        }

                        ImGui::PopID();
                    }

                }
            }

            ImGui::TreePop();
        }

        
        if( ImGui::TreeNode("Config Reader") )
        {
            AnimationState::Config& c = s_state.configAnim;

            if (ImGui::BeginCombo("Build Animator From Config", c.selected.c_str()))
            {
                FileManager* fm = FileManager::Get();
                std::vector<BasicString> file_names = fm->fileNamesInFolder(FileManager::Config_Objects);

                for( u32 i = 0; i < file_names.size(); i++ )
                {
                    const bool is_selected =  StringCompare(c.selected.c_str(), file_names[i].c_str());

                    if (ImGui::Selectable(file_names[i].c_str(), is_selected))
                    {
                        c.selected = file_names[i].c_str();
                        c.animator = ECS::Animator();
                        AnimationReader::BuildAnimatior(c.selected.c_str(), c.animator.animations);
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            // animation player/reader
            if( c.animator.animations.size() > 0 )
            {
                ImGui::PushID("config selector");

                const char* select_animation_string = actionToString(c.animator.ActiveAnimation().action).c_str();
                if (ImGui::BeginCombo("Select Animation", select_animation_string))
                {
                    for( u32 i = 0; i < c.animator.animations.size(); i++ )
                    {
                        const char* action_string = actionToString(c.animator.animations[i].action).c_str();

                        const bool is_selected =  StringCompare(action_string, select_animation_string);

                        if (ImGui::Selectable(action_string, is_selected))
                        {
                            ActionState action = stringToAction(action_string);
                            c.animator.StartAnimation(action);
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }

                    ImGui::EndCombo();
                }

                if (ImGui::Button("<- Previous Frame"))
			    {
                    c.animator.state = TimeState::Paused;

                    c.animator.frameIndex--;
                    if(c.animator.frameIndex < 0)
                    {
                        const ECS::Animation& active_animation = c.animator.ActiveAnimation();
                        c.animator.frameIndex = active_animation.frameCount - 1;
                    }
			    }

                bool is_playing = c.animator.state == TimeState::Running;
                const char* text = is_playing ? "Pause" : "Play";
                ImGui::SameLine();
                if( ImGui::Button(text) )
                {
                    if(is_playing)
                        c.animator.state = TimeState::Paused;
                    else
                        c.animator.state = TimeState::Running;
                }

                ImGui::SameLine();
			    if (ImGui::Button("Next Frame ->"))
			    {
                    c.animator.state = TimeState::Paused;

                    c.animator.frameIndex++;
                    const ECS::Animation& active_animation = c.animator.ActiveAnimation();
                    c.animator.frameIndex = c.animator.frameIndex % active_animation.frameCount;
			    }

                ECS::AnimationSystem::UpdateAnimator(c.animator, fc.delta()); 		

                ECS::Sprite sprite;
                c.animator.SetActiveSpriteFrame(sprite);

                const ECS::Animation& selected_animation = c.animator.ActiveAnimation();

			    VectorF dim = selected_animation.spriteSheet->texture->originalDimentions;
                const VectorF real_frame_size = dim / selected_animation.spriteSheet->sheetSize.toFloat();
            
                VectorF frame_texture_size(window_size.x, (window_size.x * real_frame_size.y) / real_frame_size.x);
                RectF renderFrameRect(draw_point_TL, frame_texture_size);
                draw_point_TL += VectorF(0, frame_texture_size.y) + y_spacing;

                RenderPack frame_pack(sprite.texture, renderFrameRect, 1);
                frame_pack.subRect = sprite.subRect;
			    rm->AddRenderPacket(frame_pack);

                DebugDraw::RectOutline(renderFrameRect, Colour::Yellow);
            
                ImGui::PopID();
            }

            ImGui::TreePop();
        }

        AnimationState::CursorSelection& cs = s_state.cursorSelection;
        if(im->isCursorPressed(Cursor::Left))
        {
            cs.topLeft = im->cursorPosition();
            cs.botRight.zero();
        }

        if(im->isCursorHeld(Cursor::Left))
        {
            cs.botRight = im->cursorPosition();

            VectorF size = cs.botRight - cs.topLeft;
            size.x = std::abs(size.x);
            size.y = std::abs(size.y);

            if(size.isPositive())
            {
                VectorF top_left;
                top_left.x = std::min(cs.topLeft.x, cs.botRight.x);
                top_left.y = std::min(cs.topLeft.y, cs.botRight.y);

                cs.selectionRect = RectF(top_left, size);
            }
        }
        
        DebugDraw::RectOutline( cs.selectionRect, Colour::Green);
        ImGui::VectorText("Position", cs.selectionRect.TopLeft());
        ImGui::VectorText("Size", cs.selectionRect.Size());

        // display relative position to the whole sprite

        ImGui::End();
	}
    
	void Render()
    {
	    const VectorF window_size = GameData::Get().window->size();
	    RenderManager* rm = GameData::Get().renderManager;

	    RectF screen(VectorF::zero(), window_size);
	    Texture* bg = TextureManager::Get()->getTexture( "EditorBg", FileManager::Image_UI );
	    RenderPack pack(bg, screen, 0);
	    rm->AddRenderPacket(pack);
    }
}