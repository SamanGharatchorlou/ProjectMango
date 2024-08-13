#include "pch.h"
#include "PauseState.h"

#include "Audio/AudioManager.h"
#include "Graphics/Renderer.h"

#include "Graphics/TextureManager.h"
#include "Graphics/RenderManager.h"
#include "UI/UIManager.h"

void PauseState::Init() 
{
	GameData::Get().uiManager->setCursorTexture(TextureManager::Get()->getTexture("UICursor", FileManager::Image_UI));

	AudioManager* audio = AudioManager::Get();
	AudioManager::Get()->push(AudioEvent(AudioEvent::FadeInMusic, "Menu", nullptr, 750));
}


void PauseState::HandleInput()
{

}

void PauseState::Update(float dt)
{

}

void PauseState::Exit()
{
	AudioManager::Get()->push(AudioEvent(AudioEvent::FadeOut, "Menu", nullptr, 150));
}