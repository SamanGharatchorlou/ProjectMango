#include "pch.h"
#include "AnimationEditorState.h"
#include "System/Window.h"
#include "Graphics/TextureManager.h"
#include "Graphics/RenderManager.h"
#include "Input/InputManager.h"

#include "imgui.h"
#include "Debugging/AnimationEditor.h"
#include "Graphics/Texture.h"

void AnimationEditorState::Init()
{	

}

void AnimationEditorState::HandleInput()
{
	InputManager* im = GameData::Get().inputManager;

	VectorF pos = im->cursorPosition();

}

void AnimationEditorState::Update(float dt) 
{
	//s_state.HandleInput();
	
	AnimationEditor::Render();
}

void AnimationEditorState::Exit()
{

}