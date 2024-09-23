#include "pch.h"
#include "AnimationEditor.h"

#include "ECS/EntityCoordinator.h"
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
#include "ECS/Components/Collider.h"

#include "Core/Helpers.h"
#include "imgui.h"

namespace AnimationEditor
{
    struct AnimationState
	{
		StringBuffer64 selectedSpriteSheet;
		VectorI frameCounts = VectorI(10,9);

        VectorI previousSelectedFrameIndex;
		std::vector<VectorI> selectedFrameIndexes;

        float drawHeight = 0.0f;
        float screenSizeFactor = 1.0f;
        
        struct Config
        {
            StringBuffer64 selected;

            ECS::Entity entity;
            ECS::Animator animator;
            ECS::Sprite sprite;

            TimeState state;
            int pausedFrame;
        };

        struct CursorSelection
        {
            VectorF topLeft;
            VectorF botRight;

            RectF selectionRect;

            bool movingSelection = false;
            VectorF cursorOffset;
        };

        CursorSelection cursorSelection;

        Config configAnim;

		int targetFrame = 0;
		float frameTime = 0.1f;
		float frameTimer = 0.0f;
		bool isPlayingFrames = true;
	};

    static AnimationState s_state;
    static VectorF s_targetWindowSize = VectorF(640, 640);

	void DoEditor()
	{
        s_targetWindowSize = GameData::Get().window->size() * 0.99f;

		ImGui::Begin("Animation Editor", nullptr, ImGuiWindowFlags_MenuBar);
                
        //if(s_state.configAnim.entity == ECS::EntityInvalid)
        //{
	       // ECS::EntityCoordinator* ecs = GameData::Get().ecs;
        //    s_state.configAnim.entity = ecs->CreateEntity(id);
        //}

        FrameRateController& fc = FrameRateController::Get();
	    RenderManager* rm = GameData::Get().renderManager;
        InputManager* im = GameData::Get().inputManager;
        const VectorF window_size = s_targetWindowSize;
        const VectorF y_spacing = window_size * VectorF(0.0f, 0.025f);

        VectorF draw_point_TL = y_spacing;

        ImGui::DragFloat("Resize Window", &s_state.screenSizeFactor);
        
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
                    texture_size *= s_state.screenSizeFactor;

			        RectF animation(draw_point_TL, texture_size);
                    draw_point_TL += VectorF(0, texture_size.y) + y_spacing;

			        RenderPack pack(selected_tx, animation, 1);
			        rm->AddRenderPacket(pack);

			        DebugDraw::RectOutline(animation, Colour::Yellow);
                    s_state.drawHeight = texture_size.y;

                    ImGui::VectorText("Texture Size", texture_size);
                    ImGui::InputVectorI("Frame Counts", s_state.frameCounts);

                    VectorF real_frame_size = dim / s_state.frameCounts.toFloat();
                    ImGui::VectorText("Frame Size", real_frame_size);

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
                        const VectorF cursor_pos = im->cursorScreenPosition();

                        for( u32 ix = 0; ix < frame_split_x; ix++ )
                        {   
                            for( u32 iy = 0; iy < frame_split_y; iy++ )
                            {
                                VectorF pos = frame_size * VectorI(ix,iy).toFloat() + animation.TopLeft();
                                RectF rect(pos, frame_size);
                                
                                if(PointInRect(rect, cursor_pos))
                                {
                                    VectorI index = VectorI(ix, iy);
                                    if( !Contains(s_state.selectedFrameIndexes, index) )
                                    {
                                        s_state.selectedFrameIndexes.push_back(index);
                                        s_state.previousSelectedFrameIndex = index;
                                    }
                                }
                            }
                        }
                    }

                    if(im->isCursorPressed(Cursor::ButtonType::Right))
                    {
                        s_state.selectedFrameIndexes.clear();
                        s_state.targetFrame = 0;
                    }

                    bool right = im->isPressed(Button::RightArrow);
                    bool left = im->isPressed(Button::LeftArrow);

                    if (right && s_state.selectedFrameIndexes.size() < s_state.frameCounts.x)
                    {
                        VectorI index = s_state.selectedFrameIndexes.back() + VectorI(1, 0);
                        s_state.selectedFrameIndexes.push_back(index);
                    }

                    if (left && s_state.selectedFrameIndexes.size() > 0)
                    {
                        s_state.selectedFrameIndexes.resize(s_state.selectedFrameIndexes.size() - 1);
                        s_state.targetFrame = s_state.targetFrame % s_state.selectedFrameIndexes.size();
                    }


                    bool down = im->isPressed(Button::DownArrow);
                    bool up = im->isPressed(Button::UpArrow);
                    if (down)
                    {
                        for (u32 i = 0; i < s_state.selectedFrameIndexes.size(); i++)
                        {
                            s_state.selectedFrameIndexes[i] = s_state.selectedFrameIndexes[i] + VectorI(0, 1);
                        }
                    }                   
                    if (up)
                    {
                        for (u32 i = 0; i < s_state.selectedFrameIndexes.size(); i++)
                        {
                            s_state.selectedFrameIndexes[i] = s_state.selectedFrameIndexes[i] + VectorI(0, -1);
                        }
                    }

                    for( u32 f = 0; f < s_state.selectedFrameIndexes.size(); f++ )
                    {
                        VectorI tile_index = s_state.selectedFrameIndexes[f];
                        VectorF pos = frame_size * tile_index.toFloat() + animation.TopLeft();
                        RectF rect(pos, frame_size);

                        DebugDraw::RectOutline(RectF(pos, frame_size), Colour::Purple);
                    }
                    
                    if (s_state.selectedFrameIndexes.size() > 0)
                    {
                        VectorI start_index = s_state.selectedFrameIndexes.front();
                        ImGui::Text("Start Frame Index: %d", start_index.y * s_state.frameCounts.x + start_index.x);
                    }
                    ImGui::Text("Selected Frame count: %d", s_state.selectedFrameIndexes.size());
                    ImGui::Text("Active Frame: %d", s_state.targetFrame);

                    u32 selected_count = (u32)s_state.selectedFrameIndexes.size();
                    if(selected_count > 0)
                    {
                        ImGui::PushID("sprite sheet selector");

                        // Display selected frames
                        const VectorF real_frame_size = dim / VectorI(frame_split_x,frame_split_y).toFloat();

                        VectorF frame_texture_size(window_size.x, (window_size.x * real_frame_size.y) / real_frame_size.x);
                        frame_texture_size *= s_state.screenSizeFactor;
                        VectorF adjusted_frame_texture_size = frame_texture_size;
                        adjusted_frame_texture_size.y /= selected_count;

                        VectorI top_left = s_state.selectedFrameIndexes.front();
                        VectorI bot_right = s_state.selectedFrameIndexes.back();
                        VectorF top_left_pos = real_frame_size * top_left.toFloat();
                        VectorF size = ((bot_right - top_left) + VectorI(1,1)).toFloat() * real_frame_size;

                        RectF subRect(top_left_pos, size);

                        RectF renderRect( draw_point_TL, adjusted_frame_texture_size * s_state.screenSizeFactor );
                        draw_point_TL += VectorF(0, adjusted_frame_texture_size.y) + y_spacing;

			            RenderPack pack(selected_tx, renderRect, 1);
                        pack.subRect = subRect;
			            rm->AddRenderPacket(pack);

                        DebugDraw::RectOutline(renderRect, Colour::Yellow);
                        s_state.drawHeight = renderRect.BotPoint();

                        // display the frame on the row showing the active render frame
                        VectorF render_row_frame_size = VectorF(renderRect.Width() / (float)selected_count, renderRect.Height());
                        VectorF renderRowFrame_topLeft = renderRect.TopLeft() + VectorF(s_state.targetFrame * render_row_frame_size.x, 0.f);
                        RectF renderRowFrameRect(renderRowFrame_topLeft, render_row_frame_size);
                        DebugDraw::RectOutline(renderRowFrameRect, Colour::Blue);

                        // Display Target Frame
                        ImGui::DragFloat("frame time", &s_state.frameTime);

                        if (ImGui::Button("<- Previous Frame"))
					    {
                            s_state.targetFrame--;
                            if(s_state.targetFrame < 0)
                                s_state.targetFrame = (int)selected_count - 1;

                            s_state.isPlayingFrames = false;
					    }
                        	
                        ImGui::SameLine();	
                        ImGui::Checkbox("Play Frames", &s_state.isPlayingFrames);	

                        ImGui::SameLine();
					    if (ImGui::Button("Next Frame ->"))
					    {
                            s_state.targetFrame++;
                            s_state.targetFrame = s_state.targetFrame % selected_count;

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
                        s_state.drawHeight = renderFrameRect.BotPoint();

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

        VectorF relative_selection_top_left;

        AnimationState::Config& c = s_state.configAnim;
        if( ImGui::TreeNode("Config Reader") )
        {
            if (ImGui::BeginCombo("Build Animator From Config", c.selected.c_str()))
            {
                FileManager* fm = FileManager::Get();
                std::vector<BasicString> file_names = fm->fileNamesInFolder(FileManager::Config_Animations);

                for( u32 i = 0; i < file_names.size(); i++ )
                {
                    const bool is_selected =  StringCompare(c.selected.c_str(), file_names[i].c_str());

                    if (ImGui::Selectable(file_names[i].c_str(), is_selected))
                    {
                        c.selected = file_names[i].c_str();
                        c.animator = ECS::Animator();
                        //AnimationReader::BuildAnimatior(c.selected.c_str(), c.animator.animations);
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

                const char* select_animation_string = ActionToString(c.animator.GetActiveAnimation().action);
                if (ImGui::BeginCombo("Select Animation", select_animation_string))
                {
                    for( u32 i = 0; i < c.animator.animations.size(); i++ )
                    {
                        const char* action_string = ActionToString(c.animator.animations[i].action);

                        const bool is_selected =  StringCompare(action_string, select_animation_string);

                        if (ImGui::Selectable(action_string, is_selected))
                        {
                            ActionState action = StringToAction(action_string);
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
                    if(c.animator.frameIndex == (u32)-1)
                    {
                        const ECS::Animation& active_animation = c.animator.GetActiveAnimation();
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
                    const ECS::Animation& active_animation = c.animator.GetActiveAnimation();
                    c.animator.frameIndex = c.animator.frameIndex % active_animation.frameCount;
			    }

                ECS::AnimationSystem::UpdateAnimator(c.animator, fc.delta()); 	
                
                const ECS::Animation& active_animation = c.animator.GetActiveAnimation();
                ImGui::Text("Frame %d / %d", c.animator.frameIndex + 1, active_animation.frameCount );
	

                c.animator.SetActiveSpriteFrame(c.sprite);

                if(ImGui::Button("Flip Sprite"))
                {
                    SDL_RendererFlip flip = c.sprite.flip;
                    if(flip == SDL_FLIP_HORIZONTAL)
                        c.sprite.flip = SDL_FLIP_NONE;
                    else
                        c.sprite.flip = SDL_FLIP_HORIZONTAL;
                }

                const ECS::Animation& selected_animation = c.animator.GetActiveAnimation();

			    VectorF dim = selected_animation.spriteSheet.texture->originalDimentions;
                const VectorF real_frame_size = dim / selected_animation.spriteSheet.sheetSize.toFloat();
            
                VectorF frame_texture_size(window_size.x, (window_size.x * real_frame_size.y) / real_frame_size.x);
                RectF renderFrameRect(draw_point_TL, frame_texture_size);
                draw_point_TL += VectorF(0, frame_texture_size.y) + y_spacing;

                RenderPack frame_pack(c.sprite.texture, renderFrameRect, 1);
                frame_pack.subRect = c.sprite.subRect;
                frame_pack.flip = c.sprite.flip;

                const RectF& selection_rect = s_state.cursorSelection.selectionRect;
                if(!selection_rect.Size().isZero())
                {
                    float flip_x = selection_rect.Center().x - draw_point_TL.x;
                    frame_pack.flipPoint = VectorF(flip_x, frame_texture_size.y * 0.5f);
                }

			    rm->AddRenderPacket(frame_pack);

                DebugDraw::RectOutline(renderFrameRect, Colour::Yellow);
                relative_selection_top_left = renderFrameRect.TopLeft();
            
                ImGui::PopID();
            }

            ImGui::TreePop();
        }

        AnimationState::CursorSelection& cs = s_state.cursorSelection;
        if(im->isCursorPressed(Cursor::Left))
        {
            cs.topLeft = im->cursorScreenPosition();
            cs.botRight.zero();
        }

        if(im->isCursorHeld(Cursor::Left))
        {
            cs.botRight = im->cursorScreenPosition();

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

        if(im->isCursorPressed(Cursor::Right))
        {
            cs.topLeft.zero();
            cs.botRight.zero();
            cs.selectionRect.Zero();
        }

        if(im->isHeld(Button::Shift))
        {
            const VectorF cursor_pos = im->cursorScreenPosition();
            if(!cs.movingSelection)
            {
                ECS::Collider selection;
                selection.SetBaseRect(cs.selectionRect);

                if(selection.contains(cursor_pos))
                {
                    cs.movingSelection = true;
                    cs.cursorOffset = cs.selectionRect.TopLeft() - cursor_pos;
                }
            }

            if(cs.movingSelection)
            {
                cs.selectionRect.SetTopLeft( cursor_pos + cs.cursorOffset );
            }
        }
        else
        {
            cs.movingSelection = false;
            cs.cursorOffset = VectorF::zero();
        }
        
        const RectF& selection_rect = s_state.cursorSelection.selectionRect;
        if(!selection_rect.Size().isZero())
        {
            DebugDraw::RectOutline( selection_rect, Colour::Green);
            ImGui::VectorText("Absolute Position", selection_rect.TopLeft());
            ImGui::VectorText("Absolute Size", selection_rect.Size());

            // display relative position to the whole sprite
            if( c.animator.animations.size() > 0 )
            {
                const ECS::Animation& selected_animation = s_state.configAnim.animator.GetActiveAnimation();
                const VectorF dim = selected_animation.spriteSheet.texture->originalDimentions;
                const VectorF real_frame_size = dim / selected_animation.spriteSheet.sheetSize.toFloat(); 
                const VectorF frame_texture_size(window_size.x, (window_size.x * real_frame_size.y) / real_frame_size.x);

                VectorF relative_pos = (selection_rect.TopLeft() - relative_selection_top_left) / frame_texture_size;
                VectorF relative_size = selection_rect.Size() / frame_texture_size;

                ImGui::VectorText("Relative Position", relative_pos);
                ImGui::VectorText("Relative Size", relative_size);
                            
                float flip_x = selection_rect.Center().x - draw_point_TL.x;
                ImGui::VectorText("Relative Center", VectorF(flip_x / frame_texture_size.x, 0.5f) );
            }
        }

        ImGui::End();
	}
    
	void Render()
    {
	    const VectorF window_size = GameData::Get().window->size();
	    RenderManager* rm = GameData::Get().renderManager;

	    RectF screen(VectorF::zero(), s_targetWindowSize);
	    Texture* bg = TextureManager::Get()->getTexture( "EditorBg_black", FileManager::Image_UI );
	    RenderPack pack(bg, screen, 0);
	    rm->AddRenderPacket(pack);
    }
}