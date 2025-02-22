#pragma once

#pragma warning(disable : 4018)

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define DEBUG_MODE _DEBUG

// --- Standard Library --- 

// Strings
#include <string>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <fstream>

// Containers
#include <stack>
#include <vector>
#include <map>
#include <unordered_map>
#include <queue>
#include <deque>
#include <unordered_set>

// Logging
#include <cstdarg>
#include <assert.h>

// Maths
#include <algorithm>

// Other
#include <filesystem>
/*
change the language standard in Project 
properties-> Configuration Properties-> C / C++-> Language-> C++ Language Standard 
to at least ISO C++17 Standard(/ std:c++17)
(Can also be found in Project properties->Configuration Properties->General->C++ Language Standard)
*/
namespace fs = std::filesystem;

// SDL
#include <SDL_events.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_render.h>
#include <SDL_timer.h>
#include <SDL_ttf.h>


// --- Custom ---

// Basic Containers - no dependencies
#include "Core/StringBuffers.h"
#include "Core/BasicString.h"
#include "Core/Vector2D.h"
#include "Core/Queue.h"
#include "Core/UniqueQueue.h"
#include "Core/LinkedList.h"

// General Containers - may have dependencies
#include "Core/Rect.h"
#include "Core/Quad2D.h"
#include "Core/Grid.h"
#include "Core/Colour.h"
#include "Core/Timer.h"
#include "Core/Maps.h"

// File accessors
#include "System/Files/FileManager.h"

// GAME DATA - all the gubbins, just saves me adding this everywhere I need it tbh... yes lazy
#include "Game/Data/GameData.h"

// type definitions - define this last, it may use definitions from the above files
// and they should not be using definition from this file
#include "Core/TypeDefs.h"

// Entity and Component types
#include "ECS/EntityCommon.h"

// --- Debug Only Includes ---
#if DEBUG_MODE
#include "Debugging/DebugDraw.h"
#include "Debugging/Logging.h"
#endif

// NEW operator override
// where should this go?
#define TWEAK_MEMORY_TRACKING 0 // 1 = default, 2 = (very)verbose
#define TWEAK_IGNORE_UNKNOWNS 0
#if TWEAK_MEMORY_TRACKING
#include "Debugging/MemTrack.h"
#else
#define PRINT_MEMORY 0
#endif

