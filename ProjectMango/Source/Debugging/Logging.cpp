#include "pch.h"
#include "Logging.h"

#if DEBUG_CHECK
static constexpr PriorityLevel LogLevel = PriorityLevel::Log;
#else
static constexpr PriorityLevel LogLevel = PriorityLevel::Log;
#endif

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
#if ENABLE_LOGGING
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


void DebugAssert(int lineNumber, const char* functionName, bool condition, const char* format, ...)
{
#if DEBUG_CHECK
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
#endif // DEBUG_CHECK
}