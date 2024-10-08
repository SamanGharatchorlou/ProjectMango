#pragma once

#define DEBUG_MODE _DEBUG

#if DEBUG_MODE

#define ENABLE_LOGGING 1

#define MEMORY_TRACKING 0 // 1 = default, 2 = (very)verbose
#define IGNORE_UNKNOWNS 0

// Camera
#define CAMERA_IGNORE_BOUNDARIES 0
#define PRINT_SHAKEYCAM_VALUES 0
#define SET_GAME_SCALE 0
constexpr float game_scale = 0.5f;

// Player
#define DRAW_PLAYER_RECT 1
#define DRAW_PLAYER_WEAPON_RECT 1
#define IGNORE_WALLS 0
#define IGNORED_BY_ENEMIES 1
#define INVUNERABLE 1


// Abilities
#define UNLOCK_ALL_ABILITIES 1
#define NO_ABILITY_COOLDOWNS 1
#define DRAW_ABILITY_RECTS 1

// Collectables
#define DRAW_COLLECTABLE_RECT 0


// Map
#define MAP_DEBUGGING 0
#define STARTING_LEVEL 4
#define MAP_BOUNDARIES 0
#define LABEL_TILE_INDEX 0
#define LABEL_SURFACE_RENDER_TYPES 0
#define LABEL_SURFACE_COLLISION_TYPES 0
#define LABEL_SURFACE_DECOR_TYPES 0
#define RENDER_SURFACE_TYPES (LABEL_SURFACE_RENDER_TYPES ||  LABEL_SURFACE_COLLISION_TYPES || LABEL_SURFACE_DECOR_TYPES || LABEL_TILE_INDEX)


// Enemy / AI
#define DISABLE_PATHING_OPTIMISATIONS 1
#define DRAW_AI_PATH 0
#define DRAW_AI_PATH_COSTMAP 0

#define LABEL_ENEMY_STATES 0
#define LABEL_ENEMY_HEALTH 0
#define DRAW_PLAYER_ENEMY_DISTANCE 0
#define DRAW_ENEMY_RECTS 0
#define DRAW_ENEMY_TARGET_PATH 0

#define LIMIT_ENEMY_SPAWNS 0
#define MAX_SPAWN_COUNT 40


// UI
#define UI_EDITOR 0
#define DRAW_UI_RECTS 0

#define DEBUG_CURSOR 0


// Collisions
#define TRACK_COLLISIONS 0


// Audio
#define MUTE_AUDIO 1
#define DISABLE_UI_AUDIO 0
#define PRINT_PLAYING_AUDIO 0
#define PRINT_FULL_AUDIO_CHANNELS 0
#define AUDIO_LOGGING (PRINT_PLAYING_AUDIO || PRINT_FULL_AUDIO_CHANNELS)

// NSIS Installer
#define OUTPUT_NSIS_FOLDER_INFO 0

// Networking
#define NETWORK_TESTING 1

#else
// A cap needs to be set, the framerate can be so high that dt can be 0, this can mess 
// somethings up... like the wall collisions, which is a good 'problem' to have i guess
#define FRAMERATE_CAP 240 // Use the FPS cap you want
#define PRINT_FRAMERATE_EVERY 0 // Print at the rate you specify in milliseconds
#define HIDE_CONSOLE 1
#define UNLOCK_ALL_ABILITIES 1
//#define NO_ABILITY_COOLDOWNS 1
//#define ENABLE_LOGGING 1
//#define INVUNERABLE 1
//////
//#define STARTING_LEVEL 9
////#define IGNORED_BY_ENEMIES 0
////
////#define LIMIT_ENEMY_SPAWNS 1
////#define MAX_SPAWN_COUNT 40
//
//#define PRINT_PLAYING_AUDIO 1
//#define PRINT_FULL_AUDIO_CHANNELS 1
#endif
