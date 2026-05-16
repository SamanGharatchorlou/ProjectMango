#include "pch.h"
#include "AnimationEditor.h"

#include "ECS/EntityCoordinator.h"
#include "ImGui/ImGuiHelpers.h"
#include "Graphics/TextureManager.h"
#include "Graphics/STexture.h"
#include "System/Window.h"
#include "Graphics/RenderManager.h"
#include "Input/InputManager.h"
#include "Game/FrameRateController.h"
#include "Game/Readers/AnimationReader.h"
#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntSystems/AnimationSystem.h"

#include "Core/Helpers.h"
#include "imgui.h"
#include "Debugging/ImGui/ImGuiMenu.h"

using namespace ECS;

namespace AnimationEditor
{
    struct AnimationState
	{
		StringBuffer64 selectedSpriteSheet;
		VectorI frameCounts = VectorI(9,13);

        VectorI previousSelectedFrameIndex;
		std::vector<VectorI> selectedFrameIndexes;

        float drawHeight = 0.0f;
        float screenSizeFactor = 1.0f;
        
        struct Config
        {
            StringBuffer64 selected;
            ECS::Entity entity = ECS::EntityInvalid;

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
        //Entity entity;

		int targetFrame = 0;
		float frameTime = 0.1f;
		float frameTimer = 0.0f;
		bool isPlayingFrames = true;
        bool displayCollider = false;
	};

    static AnimationState s_state;
    static VectorF s_targetWindowSize = VectorF(640, 640);

	void DoEditor()
	{
        if(!ecs->IsAlive(s_state.configAnim.entity))
        {
            s_state.configAnim.entity = CreateEntity("Editor");
            AddComponent(Animator, s_state.configAnim.entity);
            AddComponent(Sprite, s_state.configAnim.entity);
            AddComponent(Transform, s_state.configAnim.entity);
            AddComponent(Collider, s_state.configAnim.entity);
            AddComponent(EntityData, s_state.configAnim.entity);

            DebugMenu::SelectEntity(s_state.configAnim.entity);
        }

        s_targetWindowSize = GameData::Get().window->size() * 1.0f;

		ImGui::Begin("Animation Editor", nullptr, ImGuiWindowFlags_MenuBar);
                
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
            if (ImGui::BeginCombo("Sprite Sheet", s_state.selectedSpriteSheet.c_str()))
            {
                FileManager* fm = FileManager::Get();
                std::vector<BasicString> file_names = fm->fileNamesInFolder(FileManager::Images);

                std::sort(file_names.begin(), file_names.end(), [](const BasicString& a, const BasicString& b) {
                    int index = 0;

                    const char* aa = a.c_str();
                    const char* bb = b.c_str();

                    StringBuffer64 str_a(a.c_str());
                    str_a = str_a.to_lower();                    
                    StringBuffer64 str_b(b.c_str());
                    str_b = str_b.to_lower();

                    while(str_a.length() > index && str_b.length() > index)
                    {
                        if(str_a.c_str()[index] < str_b.c_str()[index])
                            return true;
                        else if(str_a.c_str()[index] > str_b.c_str()[index])
                            return false;

                        index++;
                    }
                    return str_a.length() < str_b.length();
		        });

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
                if(STexture* selected_tx = TextureManager::Get()->getTexture( s_state.selectedSpriteSheet.c_str(), FileManager::Images ))
		        {
			        VectorF dim = selected_tx->originalDimentions;
			        VectorF texture_size(window_size.x, (window_size.x * dim.y) / dim. x);
                    texture_size *= s_state.screenSizeFactor;

			        RectF animation(draw_point_TL, texture_size);
                    draw_point_TL += VectorF(0, texture_size.y) + y_spacing;

			        RenderPack pack(selected_tx, 1);
                    pack.rect = animation;
			        rm->AddRenderPacket(pack);

			        DebugDraw::RectOutline(animation, SColour::Yellow);
                    s_state.drawHeight = texture_size.y;

                    ImGui::VectorText("Texture Size", texture_size);
                    ImGui::InputVectorI("Frame Counts", s_state.frameCounts);

                    VectorF real_frame_size = dim / s_state.frameCounts.toFloat();
                    ImGui::VectorText("Frame Size", real_frame_size);

                    int frame_split_x = Maths::Max(1, s_state.frameCounts.x);
                    int frame_split_y = Maths::Max(1, s_state.frameCounts.y);
                    const VectorF frame_size = animation.Size() / VectorI(frame_split_x,frame_split_y).toFloat();
                    
                    for( u32 ix = 0; ix < frame_split_x; ix++ )
                    {
                        VectorF pointA((float)ix * frame_size.x, 0.0f);
                        VectorF pointB((float)ix * frame_size.x, texture_size.y);
                        DebugDraw::Line(pointA + animation.TopLeft(), pointB + animation.TopLeft(), SColour::Blue);
                    }
                    for( u32 iy = 0; iy < frame_split_y; iy++ )
                    {
                        VectorF pointA(0.0f, (float)iy * frame_size.y);
                        VectorF pointB(texture_size.x, (float)iy * frame_size.y);
                        DebugDraw::Line(pointA + animation.TopLeft(), pointB + animation.TopLeft(), SColour::Blue);
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
                                
                                if(Contains(rect, cursor_pos))
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

                        DebugDraw::RectOutline(RectF(pos, frame_size), SColour::Purple);
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
                        draw_point_TL += VectorF(0, adjusted_frame_texture_size.y * s_state.screenSizeFactor) + y_spacing;

			            RenderPack pack(selected_tx, 1);
                        pack.rect = renderRect;
                        pack.subRect = subRect;
			            rm->AddRenderPacket(pack);

                        DebugDraw::RectOutline(renderRect, SColour::Yellow);
                        s_state.drawHeight = renderRect.BotPoint();

                        // display the frame on the row showing the active render frame
                        VectorF render_row_frame_size = VectorF(renderRect.Width() / (float)selected_count, renderRect.Height());
                        VectorF renderRowFrame_topLeft = renderRect.TopLeft() + VectorF(s_state.targetFrame * render_row_frame_size.x, 0.f);
                        RectF renderRowFrameRect(renderRowFrame_topLeft, render_row_frame_size);
                        DebugDraw::RectOutline(renderRowFrameRect, SColour::Blue);

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
                            			            
                        RenderPack frame_pack(selected_tx, 1);
                        frame_pack.rect = renderFrameRect;
                        frame_pack.subRect = frameSubRect;
                        frame_pack.entity = s_state.configAnim.entity;
			            rm->AddRenderPacket(frame_pack);

                        DebugDraw::RectOutline(renderFrameRect, SColour::Yellow);
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
                std::vector<BasicString> file_names;// = fm->fileNamesInFolder(FileManager::Config_Animations);

                AnimationReader::Debug_GetAnimationIDs(file_names);

                for( u32 i = 0; i < file_names.size(); i++ )
                {
                    const bool is_selected =  StringCompare(c.selected.c_str(), file_names[i].c_str());

                    if (ImGui::Selectable(file_names[i].c_str(), is_selected))
                    {
                        c.selected = file_names[i].c_str();

                        RemoveComponent(Animator, s_state.configAnim.entity);
                        AddComponent(Animator, s_state.configAnim.entity);
                        AnimationReader::BuildAnimator(s_state.configAnim.entity, c.selected.c_str());

                        EntityData& ed = GetComponentRef(EntityData, s_state.configAnim.entity);
			            ed.id = c.selected.c_str();
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            // animation player/reader
            Animator* anim = GetComponent(Animator, s_state.configAnim.entity );
            if( anim && anim->IsValid() )
            {
                ImGui::PushID("config selector");

                const char* select_animation_string = ActionToString(anim->GetActiveAnimation()->action);
                if (ImGui::BeginCombo("Select Animation", select_animation_string))
                {
                    for( u32 i = 0; i < anim->animations->size(); i++ )
                    {
                        const char* action_string = ActionToString(anim->animations->at(i).action);

                        const bool is_selected =  StringCompare(action_string, select_animation_string);

                        if (ImGui::Selectable(action_string, is_selected))
                        {
                            Action::Enum action = StringToAction(action_string);
                            anim->StartAnimation(action);
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }

                    ImGui::EndCombo();
                }

                if (ImGui::Button("<- Previous Frame"))
			    {
                    anim->state = TimeState::Paused;

                    anim->frameIndex--;
                    if(anim->frameIndex == (u32)-1)
                    {
                        const ECS::Animation* active_animation = anim->GetActiveAnimation();
                        anim->frameIndex = active_animation->frameCount - 1;
                    }
			    }

                bool is_playing = anim->state == TimeState::Running;
                bool requires_restart = !anim->GetActiveAnimation()->looping && anim->OnLastFrame();

                StringBuffer32 play_pause_button_text;
                if(is_playing)
                {
                    play_pause_button_text = "Pause";
                }
                else
                {
                    if(requires_restart)
                    {
                        play_pause_button_text = "Restart";
                    }
                    else
                    {
                        play_pause_button_text = "Play";
                    }
                }

                ImGui::SameLine();
                if( ImGui::Button(play_pause_button_text.c_str()) )
                {
                    if(is_playing)
                    {
                        anim->state = TimeState::Paused;
                    }
                    else
                    {
                        anim->state = TimeState::Running;

                        // restart for looping animations
                        if(requires_restart)
                        {
                            anim->StartAnimation(anim->GetActiveAnimation()->action);
                        }
                    }
                }

                ImGui::SameLine();
			    if (ImGui::Button("Next Frame ->"))
			    {
                    anim->state = TimeState::Paused;

                    anim->frameIndex++;
                    const ECS::Animation* active_animation = anim->GetActiveAnimation();
                    anim->frameIndex = anim->frameIndex % active_animation->frameCount;
			    }

                ECS::AnimationSystem::UpdateAnimator(*anim, fc.delta()); 	
                
                const ECS::Animation* active_animation = anim->GetActiveAnimation();
                ImGui::Text("Frame %d / %d", anim->frameIndex + 1, active_animation->frameCount );
	
                Sprite& sprite = GetComponentRef(Sprite, s_state.configAnim.entity);
                //anim->SetActiveSpriteFrame(sprite);

                // FLIP
                if(ImGui::Button("Flip Sprite"))
                {
                    SDL_RendererFlip flip = sprite.params.flip;
                    if(flip == SDL_FLIP_HORIZONTAL)
                        sprite.params.flip = SDL_FLIP_NONE;
                    else
                        sprite.params.flip = SDL_FLIP_HORIZONTAL;
                }

                const ECS::Animation* selected_animation = anim->GetActiveAnimation();

			    VectorF dim = selected_animation->image.texture->originalDimentions;
                const VectorF real_frame_size = dim / selected_animation->frame.gridCount.toFloat();
            
                // the visible size of the frame you're looking at, probably the yellow box
                float x_spacing = y_spacing.y;
                VectorF frame_texture_size(window_size.x, (window_size.x * real_frame_size.y) / real_frame_size.x);
                frame_texture_size *= s_state.screenSizeFactor;
                frame_texture_size.y = frame_texture_size.y - (y_spacing.y * 2.0f);
                frame_texture_size.x = frame_texture_size.x - (x_spacing * 2.0f);
                
                RectF renderFrameRect(draw_point_TL + VectorF(x_spacing,0), frame_texture_size);
                
                // todo: remove all these rect and use transform instead?
                // then i can use the collider to properly get the flip point etc


                draw_point_TL += VectorF(0, frame_texture_size.y) + y_spacing;

                RenderPack frame_pack(selected_animation->image.texture, 1);
                frame_pack.rect = renderFrameRect;
                frame_pack.subRect = anim->GetActiveSubRect();// //selected_animation.frame.GetFrameRect(anim->frameIndex);// sprite.params.subRect;
                frame_pack.flip = sprite.params.flip;

                const RectF& selection_rect = s_state.cursorSelection.selectionRect;
                if(!selection_rect.Size().isZero())
                {
                    float flip_x = selection_rect.Center().x - renderFrameRect.LeftPoint();
                    frame_pack.flipPoint = VectorF(flip_x, frame_texture_size.y * 0.5f);
                }
                // todo: remove object center from here, moved it to collider using relative rect
                else if(const Config* config = ConfigManager::Get()->GetConfig( c.selected.c_str() ))
                {
                    Transform& transform = GetComponentRef(Transform, s_state.configAnim.entity);
                    transform.SetWorldRect(renderFrameRect.TopLeft(), renderFrameRect.Size());
                        
                    Collider& collider = GetComponentRef(Collider, s_state.configAnim.entity);
                    collider.Init();

                    frame_pack.flipPoint = transform.GetHorizontalFlipPoint();
                }

                frame_pack.entity = s_state.configAnim.entity;
			    rm->AddRenderPacket(frame_pack);

                DebugDraw::RectOutline(renderFrameRect, SColour::Yellow);
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
                top_left.x = Maths::Min(cs.topLeft.x, cs.botRight.x);
                top_left.y = Maths::Min(cs.topLeft.y, cs.botRight.y);

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

                if(selection.Contains(cursor_pos))
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
            DebugDraw::RectOutline( selection_rect, SColour::Green);
            //ImGui::VectorText("Absolute Position", selection_rect.TopLeft());
            //ImGui::VectorText("Absolute Size", selection_rect.Size());

            // display relative position to the whole sprite
            Animator* anim = GetComponent(Animator, s_state.configAnim.entity );
            if( anim && anim->IsValid() )
            {
                const ECS::Animation* selected_animation = anim->GetActiveAnimation();
                const VectorF dim = selected_animation->image.texture->originalDimentions;
                const VectorF real_frame_size = dim / selected_animation->frame.gridCount.toFloat(); 

                float x_spacing = y_spacing.y;
                VectorF frame_texture_size(window_size.x, (window_size.x * real_frame_size.y) / real_frame_size.x);
                frame_texture_size *= s_state.screenSizeFactor;
                frame_texture_size.y = frame_texture_size.y - (y_spacing.y * 2.0f);
                frame_texture_size.x = frame_texture_size.x - (x_spacing * 2.0f);

                VectorF relative_pos = (selection_rect.TopLeft() - relative_selection_top_left) / frame_texture_size;
                VectorF relative_size = selection_rect.Size() / frame_texture_size;

                ImGui::VectorText("Relative Position", relative_pos);
                ImGui::VectorText("Relative Size", relative_size);
                            
                float x_center = selection_rect.Center().x - draw_point_TL.x;
                float y_center = selection_rect.Center().y - draw_point_TL.y;
                VectorF relaive_center = relative_pos + relative_size * 0.5;

                //ImGui::VectorText("Relative Center", relaive_center );
            }
        }

        ImGui::End();
	}
    
	void Render()
    {
	    const VectorF window_size = GameData::Get().window->size();
	    RenderManager* rm = GameData::Get().renderManager;

	    RectF screen(VectorF::zero(), s_targetWindowSize);
	    STexture* black_bg = TextureManager::Get()->getTexture( "EditorBg_black", FileManager::Image_UI );
	    STexture* white_bg = TextureManager::Get()->getTexture( "EditorBg", FileManager::Image_UI );
	    RenderPack pack(white_bg, 0);
        pack.rect = screen;
	    rm->AddRenderPacket(pack);
    }

    
	void Exit()
    {
        if(ecs->IsAlive(s_state.configAnim.entity))
        {
            ecs->entities.KillEntity(s_state.configAnim.entity);
        }
    }
}