#include "pch.h"
#include "GameState.h"

#include "Audio/AudioManager.h"
#include "Characters/Enemies/Enemies.h"
#include "Characters/Player/PlayerCharacter.h"
#include "ECS/Components/ComponentsSetup.h"
#include "ECS/Components/Level.h"
#include "ECS/EntityCoordinator.h"
#include "Game/Camera/Camera.h"
#include "Game/States/AnimationEditorState.h"
#include "Game/SystemStateManager.h"
#include "Input/InputManager.h"
#include "Scene/SceneParsing/LevelReader.h"
#include "System/Window.h"

void GameState::Init()
{
	ECS::RegisterAllComponents();
	ECS::RegisterAllSystems();

	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Entity entity = ecs->CreateEntity("Map_1");
	ECS::Level& level_1 = ecs->AddComponent(Level, entity);
	Level::BuildLevel(level_1, 1);

	ECS::Entity entity2 = ecs->CreateEntity("Map_2");
	ECS::Level& level_2 = ecs->AddComponent(Level, entity2);
	Level::BuildLevel(level_2, 2);

	activeLevel = entity;

	ECS::Entity player = Player::Spawn();
	ECS::Entity enemy = Enemy::Create();

	Camera* camera = Camera::Get();
	Window* window = GameData::Get().window;

	camera->xBoundary = VectorF(-10000, 100000);
	camera->yBoundary = VectorF(0, window->size().y);

	camera->setViewport(window->size());
	camera->follow(Player::Get());

	// Start Audio
	AudioManager* audio = AudioManager::Get();
	audio->push(AudioEvent(AudioEvent::FadeInMusic, "Game", nullptr, 1500));
}


void GameState::HandleInput()
{
#if DEBUG_MODE
	InputManager* input = InputManager::Get();
	if(input->isPressed(Button::F11))
	{
		GameData::Get().systemStateManager->mStates.addState(new AnimationEditorState);
	}
#endif
}


void GameState::FastUpdate(float dt)
{
	Camera::Get()->fastUpdate(dt);
}


void GameState::Update(float dt)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
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
	
    ECS::EntityCoordinator* ecs = GameData::Get().ecs;

	// shut down all systems
	for( u32 i = 0; i < ecs->systems.entSystems.size(); i++ )
	{
		delete ecs->systems.entSystems[i];
	}
	ecs->systems.entSystems.clear();

	for( u32 i = 0; i < ECS::Component::Type::Count; i++ )
	{
		delete ecs->components.componentArrays[i];
		ecs->components.componentArrays[i] = nullptr;
	}

	for( u32 i = 0; i < ecs->entities.entityIdIndex; i++ )
	{
		ecs->entities.archetypes[i] = -1;
	}
	ecs->entities.entityIdIndex = 0;
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
