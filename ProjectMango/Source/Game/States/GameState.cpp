#include "pch.h"
#include "GameState.h"

#include "Audio/AudioManager.h"
#include "ECS/Components/IncludeComponents.h"
#include "ECS/Components/ComponentsSetup.h"
#include "ECS/EntityCoordinator.h"
#include "Entities/Factory/UIEntityBuilder.h"
#include "Entities/Factory/EnemyBuilder.h"
#include "Game/Camera/Camera.h"
#include "Game/Readers/SceneReader.h"
#include "Game/States/EditorState.h"
#include "Game/SystemStateManager.h"
#include "Input/InputManager.h"
#include "System/Window.h"
#include "Debugging/ImGui/ImGuiMenu.h"
#include "Entities/Registries/CardRegistry.h"
#include "UI/UIManager.h"
#include "Game/EndBattle.h"


GameState* GameState::GetActive()
{		
	State& state = GameData::Get().systemStateManager->mStates.getActiveState();
	return dynamic_cast<GameState*>(&state);
}

int GameState::GetTurnIndex()
{
	if(GetActive())
		return GetActive()->turnIndex;

	return 0;
}

void GameState::Init()
{
	ECS::ParseGameFileData();

	CreateUICursor();

	Scene::BuildBiome("GemBiome", 0);

	CardRegistry::DrawCards();

	Camera* camera = Camera::Get();
	Window* window = GameData::Get().window;

	camera->setViewport(window->size());
	camera->targetEntity = ECS::Faction::GetPlayer();
	camera->InitShakeyCam(5.0f, VectorF(12.0,0));

	// Start Audio (disable for now)
	SoundController* sc = AudioManager::GetController();
	sc->SetMusicVolume(0);

	UIManager::Get().Init();

	// finally init all the systems
	ecs->InitSystems();
}

void GameState::HandleInput()
{
#if DEBUG_MODE
	InputManager* input = InputManager::Get();
	if(input->isPressed(Button::Zero))
	{
		EditorState* editor = new EditorState();
		editor->OpenAnimationEditor();
		GameData::Get().systemStateManager->mStates.replaceState(editor);
	}
	if (input->isPressed(Button::Nine))
	{
		EditorState* editor = new EditorState();
		editor->OpenUIEditor();
		GameData::Get().systemStateManager->mStates.replaceState(editor);
	}

	if(input->isPressed(Button::Esc))
	{
		GameData::Get().systemStateManager->mQuit = true;
	}
	if(input->isPressed(Button::R))
	{
		GameData::Get().systemStateManager->mRestart = true;
		return;
	}

	if(input->isPressed(Button::P))
	{
		using namespace ECS;

		// pause physics	
		ECS::Signature physics_signature = ArcheBit(Physics);
		ecs->ToggleSystemPaused( physics_signature );
				
		ECS::Signature transform_signature = ArcheBit(Transform);
		ecs->ToggleSystemPaused( transform_signature );

		ECS::Signature anim_signature = ArcheBit(Animator);
		ecs->ToggleSystemPaused( anim_signature );
		return;
	}
	
	if(input->isPressed(Button::F10))
	{
		DebugMenu::ToggleShow();
	}
#endif
}

static void SavePlayerState(PlayerState& player_state)
{
	ECS::Entity player = ECS::Faction::GetPlayer();
	if (const ECS::Inventory* inventory = GetComponent(Inventory, player))
	{
		player_state.relics = inventory->relics;
	}
	if (const ECS::Health* health = GetComponent(Health, player))
	{
		player_state.health = health->currentHealth;
	}
}

static void LoadPlayerState(PlayerState& player_state)
{
	ECS::Entity player = ECS::Faction::GetPlayer();
	if (ECS::Inventory* inventory = GetComponent(Inventory, player))
	{
		inventory->relics = player_state.relics;
	}
	if (ECS::Health* health = GetComponent(Health, player))
	{
		health->currentHealth = player_state.health;
	}
}

void GameState::NextBattle()
{
	SavePlayerState(playerState);
	endGameState = EndGameState();

	ecs->DestroyAllEntities();

	CreateUICursor();
	Scene::BuildBiome("GemBiome", 1);

	LoadPlayerState(playerState);

	CardRegistry::DrawCards();
}

void GameState::Update(float dt)
{
	bool was_game_over = endGameState.gameOver;

	ECS::Entity ai = ECS::Faction::GetEnemy();
	if(ECS::Health* health = GetComponent(Health, ai))
	{
		if(health->currentHealth <= 0)
			endGameState.gameOver = true;
	}

	ECS::Entity player = ECS::Faction::GetPlayer();
	if(ECS::Health* health = GetComponent(Health, player))
	{
		if(health->currentHealth <= 0)
			endGameState.gameOver = true;
	}

	if (endGameState.gameOver)
	{
		if (!was_game_over)
		{
			endGameState.showingGameOverText = true;
			CardRegistry::ResetCards();
		}

		if (endGameState.showingGameOverText)
		{
			InputManager* input = InputManager::Get();
			if (input->isPressed(Button::Space))
			{
				endGameState.showingGameOverText = false;

				endGameState.showingRelicSelectScreen = true;
				UIManager& ui_manager = UIManager::Get();
				ui_manager.OpenScreen("RelicRewardScreen");
			}
		}

		if (endGameState.showingRelicSelectScreen)
		{
			UIManager& ui_manager = UIManager::Get();
			if (!ui_manager.IsScreenOpen("RelicRewardScreen"))
			{
				endGameState.showingRelicSelectScreen = false;
				endGameState.beginNextBattle = true;
			}
		}

		if (endGameState.beginNextBattle)
		{
			NextBattle();
		}
	}

	ecs->UpdateSystems(dt);

	Camera::Get()->Update(dt);

	Cursor* cursor = GameData::Get().inputManager->getCursor();
	cursor->mode();
}

void GameState::Exit()
{
	ecs->DestroyAllEntities();
	ECS::ClearGameFileData();
}