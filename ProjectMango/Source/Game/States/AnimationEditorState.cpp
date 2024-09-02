#include "pch.h"
#include "AnimationEditorState.h"
#include "System/Window.h"
#include "Graphics/TextureManager.h"
#include "Graphics/RenderManager.h"
#include "Input/InputManager.h"

#include "Debugging/AnimationEditor.h"
#include "Graphics/Texture.h"
#include "Game/SystemStateManager.h"

#include "Debugging/ImGui/ImGuiMenu.h"
#include "Core/Timer.h"

TimerF s_timer;

void AnimationEditorState::Init()
{	
	DebugMenu::OpenEditorWindow();

	s_timer.Start();
}

void AnimationEditorState::HandleInput()
{
	InputManager* im = GameData::Get().inputManager;

	VectorF pos = im->cursorPosition();

}

void AnimationEditorState::Update(float dt) 
{
	//s_state.HandleInput();
	if (InputManager* im = GameData::Get().inputManager)
	{
		if (im->isPressed(Button::F11) && s_timer.GetSeconds() > 1.0f)
		{
			GameData::Get().systemStateManager->mStates.popState();
			return;
		}
	}

	AnimationEditor::Render();


}

void AnimationEditorState::Exit()
{
	s_timer.Stop();
}