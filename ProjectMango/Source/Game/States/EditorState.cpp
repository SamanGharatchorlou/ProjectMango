#include "pch.h"
#include "EditorState.h"
#include "System/Window.h"
#include "Graphics/TextureManager.h"
#include "Graphics/RenderManager.h"
#include "Input/InputManager.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/EntSystems/RenderSystem.h"
#include "ECS/EntSystems/TileMapSystem.h"

#include "Debugging/AnimationEditor.h"
#include "Debugging/UIEditor.h"
#include "Graphics/STexture.h"
#include "Game/SystemStateManager.h"
#include "Core/Timer.h"
#include "Game/Camera/Camera.h"
#include "Entities/Factory/UIEntityBuilder.h"


void EditorState::OpenAnimationEditor()
{
	AnimationEditor::Open();
}
void EditorState::OpenUIEditor()
{
	UIEditor::Open();
}

void EditorState::Init()
{	
	CreateUICursor();

	SDL_ShowCursor(true);

	timer.Start();

	RectF rect = Camera::Get()->GetRect();
	rect.SetTopLeft(VectorF::zero());
	Camera::Get()->SetRect(rect);
}

void EditorState::HandleInput()
{

}

void EditorState::Update(float dt) 
{
	if (InputManager* im = GameData::Get().inputManager)
	{
		if (im->isPressed(Button::Tab))
		{
			if ( UIEditor::IsOpen() )
			{
				UIEditor::Close();
				AnimationEditor::Open();
			}
			else if (AnimationEditor::IsOpen())
			{
				AnimationEditor::Close();
				UIEditor::Open();
			}
		}
	}

	if (AnimationEditor::IsOpen())
		AnimationEditor::Render();
	else if(UIEditor::IsOpen())
		UIEditor::Render();

	ECS::TileMapSystem* map_system = ecs->systems.GetSystem<ECS::TileMapSystem>();
	map_system->Update(dt);
	ECS::RenderSystem* render_sys = ecs->systems.GetSystem<ECS::RenderSystem>();
	render_sys->Update(dt);
}

void EditorState::Exit()
{
	SDL_ShowCursor(false);
	
	if (AnimationEditor::IsOpen())
		AnimationEditor::Close();
	else if (UIEditor::IsOpen())
		UIEditor::Close();

	timer.Stop();
}