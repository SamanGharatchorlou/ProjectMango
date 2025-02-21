 #include "pch.h"
#include "PreGameState.h"

#include "Audio/AudioManager.h"
#include "Graphics/TextureManager.h"

#include "ECS/EntityCoordinator.h"

void PreGameState::Init()
{	
	AudioManager* audio = AudioManager::Get();
	if(!audio->isPlaying("Menu", nullptr))
		audio->push(AudioEvent(AudioEvent::FadeInMusic, "Menu", nullptr, 1000));
}

void PreGameState::Update(float dt) 
{
	ecs->UpdateSystems(dt);
}

void PreGameState::Exit()
{
	AudioManager::Get()->push(AudioEvent(AudioEvent::FadeOut, "Menu", nullptr, 150));
}

