#include "pch.h"
#include "PauseState.h"

#include "Audio/AudioManager.h"
#include "Graphics/Renderer.h"

#include "Graphics/TextureManager.h"
#include "Graphics/RenderManager.h"

void PauseState::Init() 
{
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