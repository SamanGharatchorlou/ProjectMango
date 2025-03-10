#include "pch.h"
#include "GameState.h"

#include "Audio/AudioManager.h"
#include "Entities/Player/PlayerCharacter.h"
#include "ECS/Components/ComponentsSetup.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Biome.h"
#include "ECS/EntityCoordinator.h"
#include "Game/Camera/Camera.h"
#include "Game/States/AnimationEditorState.h"
#include "Game/SystemStateManager.h"
#include "Input/InputManager.h"
#include "Scene/SceneParsing/SceneReader.h"
#include "Scene/SceneParsing/EntityBuilder.h"
#include "Scene/SceneParsing/UIEntityBuilder.h"
#include "Entities/Spells/PickupCallbacks.h"
#include "System/Window.h"

void GameState::Init()
{
	ECS::RegisterAllComponents();
	ECS::RegisterAllSystems();

	ECS::Entity biome_entity = ECS::CreateEntity("Map_1");

	AddComponent(Biome, biome_entity);
	Scene::BuildBiome( "Biome1", biome_entity );
	activeLevel = biome_entity;

	CreateEntities(biome_entity);
	PickUps::SetCallbacks();

	Camera* camera = Camera::Get();
	Window* window = GameData::Get().window;

	camera->setViewport(window->size());
	camera->targetEntity = Player::Get();

	// Start Audio
	AudioManager* audio = AudioManager::Get();
	audio->push(AudioEvent(AudioEvent::FadeInMusic, "Game", nullptr, 1500));

	// create cursor
	CreateUIEntities();
}

void GameState::HandleInput()
{
#if DEBUG_MODE
	InputManager* input = InputManager::Get();
	if(input->isPressed(Button::Zero))
	{
		GameData::Get().systemStateManager->mStates.addState(new AnimationEditorState);
	}
	if(input->isPressed(Button::Esc))
	{
		GameData::Get().systemStateManager->mQuit = true;
	}
	if(input->isPressed(Button::R))
	{
        GameData::Get().systemStateManager->mStates.replaceState(new GameState);
		return;
	}
#endif
}


void GameState::FastUpdate(float dt)
{
	//Camera::Get()->fastUpdate(dt);
}


void GameState::Update(float dt)
{
	ecs->UpdateSystems(dt);

	Camera::Get()->Update(dt);

	Cursor* cursor = GameData::Get().inputManager->getCursor();
	cursor->mode();
}

void GameState::Resume() 
{
	//mGameData->environment->resume();
	AudioManager::Get()->push(AudioEvent(AudioEvent::FadeInMusic, "Game", nullptr, 750));
}

void GameState::Pause()
{
	//mGameData->environment->pause();
	AudioManager::Get()->push(AudioEvent(AudioEvent::FadeOut, "Game", nullptr, 150));
}


void GameState::Exit()
{
	//mGameData->environment->clear();
	//mGameData->scoreManager->reset();
	AudioManager::Get()->push(AudioEvent(AudioEvent::FadeOut, "Game", nullptr, 150));
	
	ecs->Close();// systems.Close();
}


// --- Private Functions --- //

void GameState::initCamera()
{
	//Camera* camera = Camera::Get();

	//camera->setViewport(VectorF(100.0f, 100.0f));
	//camera->follow(Player::Get());

	//VectorF cameraPosition = VectorF(0.0f, 0.0f);
	//camera->SetPosition(cameraPosition);

	// TODO: fix these values
	//camera->initShakeyCam(100.0f, 80.0f);

	//RectF* playerRect = &mGameData->environment->actors()->player()->get()->rectRef();
	//camera->follow(playerRect);
}
