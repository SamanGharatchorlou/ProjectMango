#include "pch.h"
#include "AnimationEditor.h"

#include "ImGui/ImGuiHelpers.h"
#include "Graphics/TextureManager.h"
#include "Graphics/Texture.h"
#include "System/Window.h"
#include "Graphics/RenderManager.h"
#include "Input/InputManager.h"
#include "Game/FrameRateController.h"

#include "Core/Helpers.h"
#include "imgui.h"

namespace AnimationEditor
{
    static AnimationState s_state;

	void DoEditor()
	{
		ImGui::Begin("Animation Editor", nullptr, ImGuiWindowFlags_MenuBar);
    
        // file select dropdown
        ECS::EntityCoordinator* ecs = GameData::Get().ecs;
        if (ImGui::BeginCombo("Animation", s_state.selectedAnimation.c_str(), 0))
        {
            FileManager* fm = FileManager::Get();
            std::vector<BasicString> file_names = fm->fileNamesInFolder(FileManager::Image_Animations);

            for( u32 i = 0; i < file_names.size(); i++ )
            {
                const bool is_selected =  StringCompare(s_state.selectedAnimation.c_str(), file_names[i].c_str());

                if (ImGui::Selectable(file_names[i].c_str(), is_selected))
                    s_state.selectedAnimation = file_names[i].c_str();

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }
        
        // animation editing
        if( strlen(s_state.selectedAnimation.c_str()) > 0 )
        {
            if(const Texture* selected_tx = TextureManager::Get()->getTexture( s_state.selectedAnimation.c_str(), FileManager::Image_Animations ))
		    {
	            const VectorF window_size = GameData::Get().window->size();
			    VectorF dim = selected_tx->originalDimentions;
                
                const VectorF y_spacing = window_size * VectorF(0.0f, 0.1f);

			    VectorF texture_size(window_size.x, (window_size.x * dim.y) / dim. x);
			    RectF animation(y_spacing, texture_size);
                
	            RenderManager* rm = GameData::Get().renderManager;
			    RenderPack pack(selected_tx, animation, 1);
			    rm->AddRenderPacket(pack);

			    DebugDraw::RectOutline(animation, Colour::Yellow);

                ImGui::VectorText("Texture Size", texture_size);

                ImGui::InputVectorI("Frame Size", s_state.frameCounts);

                ImGui::Checkbox("Show Frame Split", &s_state.showFrameSplit);
                if(s_state.showFrameSplit)
                {
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

                    InputManager* im = GameData::Get().inputManager;
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
                        RectF renderRect( animation.BotLeft() + y_spacing, adjusted_frame_texture_size );

			            RenderPack pack(selected_tx, renderRect, 1);
                        pack.subRect = subRect;
			            rm->AddRenderPacket(pack);

                        DebugDraw::RectOutline(renderRect, Colour::Yellow);

                        // Display Target Frame
                        ImGui::DragFloat("frame time", &s_state.frameTime);

                        if (ImGui::Button("<- Previous Frame"))
					    {
                            s_state.targetFrame--;
                            if(s_state.targetFrame < 0)
                                s_state.targetFrame = (int)s_state.selectedFrameIndexes.size() - 1;

                            s_state.isPlayingFrames = false;
					    }
                        		
                        ImGui::Checkbox("Play Frames", &s_state.isPlayingFrames);	

                        ImGui::SameLine();
					    if (ImGui::Button("Next Frame ->"))
					    {
                            s_state.targetFrame++;
                            s_state.targetFrame = s_state.targetFrame % s_state.selectedFrameIndexes.size();

                            s_state.isPlayingFrames = false;
					    }

                        RectF renderFrameRect(renderRect.BotLeft() + y_spacing, frame_texture_size);

                        VectorI frame_top_left_index = s_state.selectedFrameIndexes[s_state.targetFrame];
                        VectorF frame_top_left_pos = real_frame_size * frame_top_left_index.toFloat(); 
                        RectF frameSubRect(frame_top_left_pos, real_frame_size);
                            			            
                        RenderPack frame_pack(selected_tx, renderFrameRect, 1);
                        frame_pack.subRect = frameSubRect;
			            rm->AddRenderPacket(frame_pack);

                        DebugDraw::RectOutline(renderFrameRect, Colour::Yellow);

                        // display the frame on the row showing the active render frame
                        VectorF renderRowFrame_topLeft = renderRect.TopLeft() + VectorF(s_state.targetFrame * real_frame_size.x, 0.f);
                        RectF renderRowFrameRect(renderRowFrame_topLeft, real_frame_size);
                        DebugDraw::RectOutline(renderRowFrameRect, Colour::Blue);

                        if(s_state.isPlayingFrames)
                        {
                            FrameRateController& fc = FrameRateController::Get();

                            s_state.frameTimer += fc.delta();

                            if(s_state.frameTimer > s_state.frameTime)
                            {
                                s_state.targetFrame++;
                                s_state.targetFrame = s_state.targetFrame % s_state.selectedFrameIndexes.size();

                                s_state.frameTimer = 0.0f;
                            }
                        }
                    }
                }
            }
        }

        ImGui::End();
	}

    
	void HandleInput()
    {

    }

	AnimationState& GetAnimationState()
    {
        return s_state;
    }
    
	void AnimationState::Render()
    {
	    const VectorF window_size = GameData::Get().window->size();
	    RenderManager* rm = GameData::Get().renderManager;

	    RectF screen(VectorF::zero(), window_size);
	    Texture* bg = TextureManager::Get()->getTexture( "EditorBg", FileManager::Image_UI );
	    RenderPack pack(bg, screen, 0);
	    rm->AddRenderPacket(pack);
    }

    void AnimationState::HandleInput()
    {

    }
}