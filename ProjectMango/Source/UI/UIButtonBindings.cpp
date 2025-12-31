#include "pch.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"

using namespace ECS;

static void SetupCoinBindings(std::unordered_map<BasicString, std::function<void(ECS::Entity)>>& button_bindings)
{
	button_bindings[ "CoinStack" ] =  [](ECS::Entity entity) {
			ActionRequest& action_request = AddComponent(ActionRequest, Faction::GetPlayer());
			action_request.request = ActionRequest::CollectCoin;
			action_request.target = entity; 
		};
}

void SetupButtonActionBindings(std::unordered_map<BasicString, std::function<void(ECS::Entity)>>& button_bindings)
{
	SetupCoinBindings(button_bindings);

	button_bindings[ "RequestCoin" ] =  [](ECS::Entity entity) {
			ActionRequest& action_request = AddComponent(ActionRequest, Faction::GetPlayer());
			action_request.request = ActionRequest::CollectCoin;
			action_request.target = entity; 
		};

	button_bindings[ "RequestCard" ] =  [](ECS::Entity entity) {
			ActionRequest& action_request = AddComponent(ActionRequest, Faction::GetPlayer());
			action_request.request = ActionRequest::AquireCard;
			action_request.target = entity;
		};

	button_bindings[ "EndTurnButton" ] =  [](ECS::Entity) {
		if(TurnState* turn_state = GetComponent(TurnState, Faction::GetPlayer()))
		{
			turn_state->canEndTurn = true;
		} };
		
	button_bindings[ "UndoTurnButton" ] =  [](ECS::Entity entity) {
			ActionRequest& action_request = AddComponent(ActionRequest, Faction::GetPlayer());
			action_request.request = ActionRequest::UndoTurn;
			action_request.target = entity; 
		};

	button_bindings[ "AutoConfirmTurn" ] =  [](ECS::Entity entity) {
		Sprite& sprite = GetComponentRef(Sprite, entity);
		UIButton& button = GetComponentRef(UIButton, entity);
		sprite.SetTexture(button.toggle ? "tick" : "cross");

		State& state = GameData::Get().systemStateManager->mStates.getActiveState();
		if(GameState* game_state = dynamic_cast<GameState*>(&state))
		{
			game_state->autoConfirmTurn = button.toggle;
		}
	};
}

void SetupButtonUIBindings(std::unordered_map<BasicString, std::function<void(ECS::Entity)>>& button_bindings)
{
	button_bindings[ "EndTurnButton" ] =  [](ECS::Entity entity) {
		if(const TurnState* turn_state = GetComponent(TurnState, Faction::GetPlayer()))
		{
			Sprite& sprite = GetComponentRef(Sprite, entity);
			if(turn_state->CanAquireMoreResources())
				sprite.params.colourMod = SColour::White;
			else
				sprite.params.colourMod = SColour::Green;
		} };
	button_bindings[ "UndoTurnButton" ] =  [](ECS::Entity entity) {
		if(const TurnState* turn_state = GetComponent(TurnState, Faction::GetPlayer()))
		{
			Sprite& sprite = GetComponentRef(Sprite, entity);
			if(turn_state->HasAquiredResources())
				sprite.params.colourMod = SColour::Green;
			else
				sprite.params.colourMod = SColour::White;
		} };
}