#include "pch.h"
#include "Logging.h"

#if DEBUG_MODE
static constexpr PriorityLevel LogLevel = PriorityLevel::Debug;
#else
static constexpr PriorityLevel LogLevel = PriorityLevel::Log;
#endif

#define TWEAK_ENABLE_LOGGING 1

static std::unordered_map<BasicString, bool> s_loggings;

static void PriorityLevelToText(PriorityLevel level, BasicString& out_text)
{
	switch (level)
	{
	case Error:
		out_text = "Error";
		break;
	case Warning:
		out_text = "Warning";
		break;
	case Profile:
		out_text = "Profile";
		break;
	case Debug:
		out_text = "Debug";
		break;
	case Log:
	case None:
	default:
		break;
	}
}

void DebugPrint(PriorityLevel priority, const char* format, ...)
{
#if TWEAK_ENABLE_LOGGING
	if (priority <= LogLevel)
	{
		va_list arg;

		BasicString log_level;
		PriorityLevelToText(priority, log_level);
		if(!log_level.empty())
			fprintf(stdout, "%s: ", log_level.c_str());

		va_start(arg, format);
		vfprintf(stdout, format, arg);
		va_end(arg);

		fprintf(stdout, "\n");
	}
#endif
}

void DebugPrintOnce(PriorityLevel priority, const char* format, ...)
{
#if TWEAK_ENABLE_LOGGING
	if (priority <= LogLevel)
	{
		va_list arg;
		va_start(arg, format);

		va_list args_copy;
		va_copy(args_copy, arg);

		// use args_copy for size calculation
        int len = vsnprintf(nullptr, 0, format, args_copy);
        va_end(args_copy);

		BasicString string;
		string.setNewBuffer(len);

		vsnprintf(string.buffer(), string.bufferLength(), format, arg);
		va_end(arg);

		if(s_loggings.contains(string))
			return;

		s_loggings[string] = true;

		BasicString log_level;
		PriorityLevelToText(priority, log_level);
		if(!log_level.empty())
			fprintf(stdout, "%s: ", log_level.c_str());

		fprintf( stdout, "%s\n", string.c_str() );
	}
#endif
}


void DebugAssert(int lineNumber, const char* functionName, bool condition, const char* format, ...)
{
#if DEBUG_MODE
	if ( !condition )
	{
		printf("Assert failed in %s::%d - ", functionName, lineNumber);

		va_list arg;

		va_start(arg, format);
		vfprintf(stdout, format, arg);
		va_end(arg);

		// pause the code
		int a; 
		scanf_s("%d", &a);

		// either properly pause the code or open an output window with a failed message
	}
#endif // DEBUG_MODE
}