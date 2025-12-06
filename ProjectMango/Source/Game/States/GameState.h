#pragma once

#include "Game/States/State.h"

class GameState : public State
{
public:
	void Init() override;
	void Update(float dt) override;
	void FastUpdate(float dt) override;
	void HandleInput() override;
	
	void Pause() override;
	void Resume() override;
	void Exit() override;
	
	ECS::Entity activeLevel = ECS::EntityInvalid;

private:
	void initCamera();
};

// how to handle the movement and action state
// need something to tell the player what state they're in, default to movement probably best but allow them to select the other
// button or something or........ why not make it more free flowing essntially allow them to play for a short time, but not time
// its actually over a short movement so they can run and jump, shoot at some point then keep moving, freeze them once they run out of
// movement, this means they'll be stuck in the air and have forced movement the next turn, cool

// need a way to make this play out, so there needs to be a simulate or play button to allow the movement to progress then can keep pushing
// a direction then once they land they can roll or something to avoid the next incoming attack

// how to handle these states
// need to pause physics, 
// for the whole game, prevent any other interactions progressing as well, whatever they might be, e.g. a project or something
// in fact physics should be paused for everything and i need to be more careful with what i turn on and off

// what kind of entities use this system
// player, enemies, projectiles, attack moves, even a box
// does it need to be a part, something that handles the time component of all this, maybe


// Turns
// player has a turn, then enemies have a turn, or does it go player, enemy1, player, enemy2 - probably not
// during a turn player has a movement point MP and an action point AP
// during player turn player can freely move and shoot, but moving consumes movement so there has to be an undo
// once happy they confirm and it changes to enemy turn
// here an AIcontroll will take over, make the enemies take their turns however they want, this should just happen and it
// all plays out once the players confirms

// is the turn a part, something I give to the player and enemies? they get a turn order or some initiative value
// I can add and remove this freely, so the player kills the enemies, then i remove the turn part and the game returns to normal flow
// in the game controller if there's an entity with a turn part were in simulation mode

// is simulation mode a new game state that sits on top of the game state or just something that happens within the game state
// i think just dump it into the game state
// while in simulation mode pause physics and only play once the player has had their turn


// UI required
// show the MP and AP, MP is a bar that empties and the MP is just a filled in or empty box
// undo button

// player predicted movement
// say player walks then jumps, they only get so far until they run out of MP
// we need a way to predict the movement (as best we can) so the enemy can attach them where they land

// this is gameplay only whats the core loop
// unlock stuff - weapons, armor, health, more action points, more movement distance
// level up to improve base damage, health, speed (MP), inititive if high enough they could get 2 turns maybe
// getting another action point should be a big base upgrade, beating a boss or something





// SPLENDOR
// building an engine, collect coins, buy cards, cards help buy more cards
// this has threads of magic, get mana, play lands, helps you play more stuff

// what are the coins, what are the cards, what are the enemies and what do they do

// are the cards an army, you can pick workers, fighters, defenders
// each has a faction, e.g. a red worker will give you a red gem, to collect more red cards
// workers are cheap, fights are stronger and deal damage, each turn or just as you buy one, workers attack too, just not much
// have to balance buying lots of cheap stuff vs expensive ones while balancing dealing damage

// enemies are simple monsters that fight, keep it simple for now