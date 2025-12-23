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
#include "Game/Readers/SceneReader.h"
#include "Entities/EntityBuilder.h"
#include "Entities/UIEntityBuilder.h"
#include "System/Window.h"
#include "ECS/Components/Physics.h"

void GameState::Init()
{
	ECS::RegisterAllComponents();
	ECS::RegisterAllSystems();
	ECS::ParseComponentData();

	ECS::Entity biome_entity = ECS::CreateEntity("Map_1");

	AddComponent(Biome, biome_entity);
	Scene::BuildBiome( "GemBiome_old", biome_entity );
	activeLevel = biome_entity;

	ECS::EntityMetaData empty_data;
	ECS::Entity player = Player::Spawn(empty_data);
	
	CreateEntities(biome_entity);

	ECS::EntityState& player_state = GetComponentRef(EntityState, player);

	Camera* camera = Camera::Get();
	Window* window = GameData::Get().window;

	camera->setViewport(window->size());
	camera->targetEntity = Target::GetPlayer();
	camera->InitShakeyCam(5.0f, VectorF(12.0,0));

	// Start Audio
	AudioManager* audio = AudioManager::Get();
	audio->PlayMusic("Game");

	SoundController* sc = AudioManager::GetController();
	sc->SetMusicVolume(0);//0.05f);
	//audio->push(AudioEvent(AudioEvent::FadeInMusic, "Game", nullptr, 1500));
	//float vol = audio->musicVolume();
	//audio->setMusicVolume(0.2f);

	// create cursor
	CreateUIEntities();

	// finally init all the systems
	ecs->InitSystems();
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

	if(input->isPressed(Button::P))
	{
		// pause physics	
		ECS::Signature physics_signature = ArcheBit(Physics);
		ecs->ToggleSystemPaused( physics_signature );
				
		ECS::Signature transform_signature = ArcheBit(Transform);
		ecs->ToggleSystemPaused( transform_signature );
		return;
	}
}


void GameState::FastUpdate(float dt)
{
	//Camera::Get()->fastUpdate(dt);
}


void GameState::Update(float dt)
{
	ECS::Entity ai = ECS::Target::GetEnemy();
	if(ECS::Health* health = GetComponent(Health, ai))
	{
		if(health->currentHealth <= 0)
			gameOver = true;
	}

	ECS::Entity player = Target::GetPlayer();
	if(ECS::Health* health = GetComponent(Health, player))
	{
		if(health->currentHealth <= 0)
			gameOver = true;
	}

	ecs->UpdateSystems(dt);

	Camera::Get()->Update(dt);

	Cursor* cursor = GameData::Get().inputManager->getCursor();
	cursor->mode();
}

void GameState::Resume() 
{
	//mGameData->environment->resume();
	//AudioManager::Get()->push(AudioEvent(AudioEvent::FadeInMusic, "Game", nullptr, 750));
}

void GameState::Pause()
{
	//mGameData->environment->pause();
	//AudioManager::Get()->push(AudioEvent(AudioEvent::FadeOut, "Game", nullptr, 150));
}


void GameState::Exit()
{
	//mGameData->environment->clear();
	//mGameData->scoreManager->reset();
	//AudioManager::Get()->push(AudioEvent(AudioEvent::FadeOut, "Game", nullptr, 150));
	
	ecs->Close();// systems.Close();
}


// --- Private Functions --- //

void GameState::initCamera()
{
	//Camera* camera = Camera::Get();

	//camera->setViewport(VectorF(100.0f, 100.0f));
	//camera->follow(Target::GetPlayer());

	//VectorF cameraPosition = VectorF(0.0f, 0.0f);
	//camera->SetPosition(cameraPosition);

	// TODO: fix these values
	//camera->initShakeyCam(100.0f, 80.0f);

	//RectF* playerRect = &mGameData->environment->actors()->player()->get()->rectRef();
	//camera->follow(playerRect);
}
