#include "pch.h"
#include "AnimationEditorState.h"
#include "System/Window.h"
#include "Graphics/TextureManager.h"
#include "Graphics/RenderManager.h"
#include "Input/InputManager.h"

#include "Debugging/AnimationEditor.h"
#include "Graphics/STexture.h"
#include "Game/SystemStateManager.h"

#include "Debugging/ImGui/ImGuiMenu.h"
#include "Core/Timer.h"
#include "Game/Camera/Camera.h"

void AnimationEditorState::Init()
{	
	SDL_ShowCursor(true);
	//SDL_SetRelativeMouseMode(SDL_FALSE);

	DebugMenu::OpenEditorWindow();

	timer.Start();

	RectF rect = Camera::Get()->GetRect();
	rect.SetTopLeft(VectorF::zero());
	Camera::Get()->SetRect(rect);
}

void AnimationEditorState::HandleInput()
{

}

void AnimationEditorState::Update(float dt) 
{
	//s_state.HandleInput();
	if (InputManager* im = GameData::Get().inputManager)
	{
		bool f11 = im->isPressed(Button::F11);
		bool zero = im->isPressed(Button::Zero);
		if ( (f11 || zero) && timer.GetSeconds() > 1.0f )
		{
			GameData::Get().systemStateManager->mStates.popState();
			return;
		}
	}
	
	//AnimationEditor::DoEditor();
	AnimationEditor::Render();
}

void AnimationEditorState::Exit()
{
	SDL_ShowCursor(false);
	//SDL_SetRelativeMouseMode(SDL_TRUE);

	
	AnimationEditor::Exit();

	timer.Stop();
}