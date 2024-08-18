#pragma once

enum PriorityLevel
{
	None = -1,
	Error,		// stop program
	Warning,	// something failed
	Profile,
	Log,		// basic info
	Debug		// custom debugging
};


#if DEBUG_MODE
#define ASSERT(...) DebugAssert(__LINE__, __FUNCTION__, __VA_ARGS__)
#else
#define ASSERT(...)
#endif


void DebugPrint(PriorityLevel priority, const char* format, ...);
void DebugAssert(int lineNumber, const char* functionName, bool condition, const char* format, ...);