#include "pch.h"
#include "ShockSweeperEnemy.h"

#include "Core/Helpers.h"
#include "ECS/Components/AIComponents.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Physics.h"
#include "ECS/EntityCoordinator.h"
#include "Game/FrameRateController.h"
#include "ECS/Components/GameComponents.h"

#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"
#include "Entities/EntityBuilder.h"

#include "Debugging/ImGui/ImGuiMainWindows.h"

namespace ShockSweeper
{
	using namespace ECS;
	
	// Enemy
	// ---------------------------------------------------------
	// create an actual enemy i.e. the thing the player fights
	Entity CreateEnemy(const ECS::EntityMetaData& emd)
	{
		Entity entity = CreateActor(emd);
		const Config* config = ECS::GetConfigFromEntity(entity);

		AIController& ai = AddComponent(AIController, entity);
		ai.isDisabled = true;
			
		AddComponent(AIIntent, entity);

		// Inventory
		Inventory& inventory = AddComponent(Inventory, entity);

		// Turn
		TurnState& turn = AddComponent(TurnState, entity);
		turn.initiative = 10;

		// Pathing
		bool disable_pathing = config->data.GetBool("disable_pathing", false);
		if(!disable_pathing)
		{
			Pathing& pathing = AddComponent(Pathing, entity);
			//pathing.Init();
		}

		// mark ourselves as the enemy
		State& state = GameData::Get().systemStateManager->mStates.getActiveState();
		if(GameState* game_state = dynamic_cast<GameState*>(&state))
		{
			game_state->enemy = entity;
		}

		//Target& target = AddComponent(Target, entity);
		//target.target = Target::GetPlayer();
		
		return entity;
	}

	//void Enemy::Begin(ECS::Entity entity)
	//{
	//	EntityState& state = GetComponentRef(EntityState, entity);

	//	if(!state.actions.HasAction())
	//		PushState(Idle);
	//}

	//bool Enemy::FinishedDying(ECS::Entity entity)
	//{
	//	EntityState& state = GetComponentRef(EntityState, entity);

	//	if(DeathState* death_state = static_cast<DeathState*>(&(state.actions.Top())))
	//		return death_state->can_kill;

	//	return false;
	//}

	//void Enemy::StartDying(ECS::Entity entity)
	//{
	//	EntityState& state = GetComponentRef(EntityState, entity);

	//	state.actions.Pop();
	//	state.actions.Push( new DeathState(entity) );
	//	state.actions.Push( new TakeHitState(entity) );
	//}


//	// Idle
//	// ---------------------------------------------------------
//	void IdleState::Init()
//	{
//		StartAnimation();
//	}
//	void IdleState::Update(float dt)
//	{
//
//	}
//
//	// TakeHitState
//	// ---------------------------------------------------------
//	void TakeHitState::Init()
//	{
//		StartAnimation();
//	}
//
//	void TakeHitState::Update(float dt)
//	{	
//		const Animator& animator = GetComponentRef(Animator, entity);
//
//		if(animator.loopCount > 0)
//		{
//			EntityState& state = GetComponentRef(EntityState, entity);
//			PopState();
//			return;
//		}
//	}
//
//	// DeathState
//	// ---------------------------------------------------------
//	void DeathState::Init()
//	{
//		StartAnimation(false);
//
//		can_kill = false;
//		
//		InitDeathState();
//	}
//
//	void DeathState::Update(float dt)
//	{	
//		const Animator& animation = GetComponentRef(Animator, entity);
//
//		if(animation.loopCount > 0)
//			can_kill = true;
//	}
//
//	// BasicAttack
//	// ---------------------------------------------------------
//	BasicAttackState::BasicAttackState(ECS::Entity _entity) : CharacterAction(ActionState::BasicAttack, _entity) { }
//
//	void BasicAttackState::Init()
//	{
//		StartAnimation();
//	}
//
//	void BasicAttackState::Update(float dt)
//	{
//		EntityState& state = GetComponentRef(EntityState, entity);
//		Animator& animator = GetComponentRef(Animator, entity);
//		
//		if(animator.loopCount > 0)
//		{
//			AIController& ai_controller = GetComponentRef(AIController, entity);
//			ai_controller.cooldownTimer.Start();
//
//			PopState();
//			return;
//		}
//	}
//
//	void BasicAttackState::Exit()
//	{
//		ecs->entities.KillEntity(attackCollider);
//	}
//}
//
//
//namespace TrainingDummy
//{
//	using namespace ECS;
//	
//	// Enemy
//	// ---------------------------------------------------------
//	Entity Create(const EntityMetaData& emd)
//	{
//		Entity entity = Character::CreateMonster(emd);
//				
//		// EntityState
//		EntityState& character_state = AddComponent(EntityState, entity);
//		character_state.character = new Enemy();
//
//		return entity;
//	}
//
//	void Enemy::Begin(ECS::Entity entity)
//	{
//		EntityState& state = GetComponentRef(EntityState, entity);
//
//		if(!state.actions.HasAction())
//			PushState(Idle);
//	}
//
//	// Idle
//	// ---------------------------------------------------------
//	void IdleState::Init()
//	{
//		StartAnimation();
//	}
//	void IdleState::Resume()
//	{		
//		
//		AIController& ai_controller = GetComponentRef(AIController, entity);
//		bool can_flip_sprite = !ai_controller.cooldownTimer.IsRunning();
//		
//		StartAnimation(can_flip_sprite);
//	}
//	void IdleState::Update(float dt)
//	{
//
//	}
//
//	
//	// TakeHitState
//	// ---------------------------------------------------------
//	void TakeHitState::Init()
//	{
//		StartAnimation();
//		
//		const FrameRateController& frc = FrameRateController::Get();
//		frameStart = frc.FrameCount();
//	}
//
//	void TakeHitState::Update(float dt)
//	{	
//		
//		const Animator& animator = GetComponentRef(Animator, entity);
//		EntityState& state = GetComponentRef(EntityState, entity);
//
//		if(animator.loopCount > 0)
//		{
//			PopState();
//			return;
//		}
//		
//		const FrameRateController& frc = FrameRateController::Get();
//		if( (frameStart + c_hitFrameBuffer) < frc.FrameCount())
//		{
//			if(const Collider* collider = GetComponent(Collider, entity))
//			{
//				if(collider->lastHitFrame != -1 && (collider->lastHitFrame + 20) >= frc.FrameCount())
//				{
//					ReplaceState(TakeHit);
//				}
//			}
//		}
//	}
//
//	void TakeHitState::Exit()
//	{
//		
//		if(Collider* collider = GetComponent(Collider, entity))
//		{
//			//collider->lastHitFrame = -1;
//		}
//	}
}