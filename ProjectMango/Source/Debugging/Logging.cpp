#include "pch.h"
#include "Logging.h"

#if DEBUG_MODE
static constexpr PriorityLevel LogLevel = PriorityLevel::Debug;
#else
static constexpr PriorityLevel LogLevel = PriorityLevel::Log;
#endif

static std::unordered_map<BasicString, bool> s_loggings;

static void PriorityLevelToText(PriorityLevel level, StringBuffer64& out_text)
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
		// ------ temp to find a bug -------
		if (!format)
		{
			fprintf(stdout, "DebugPrint: null format\n");
			fflush(stdout);
			return;
		}

		fprintf(stdout, "DebugPrint: ptr=%p\n", (void*)format);
		fflush(stdout);
		// ------ temp to find a bug -------

		// Validate the pointer before touching it
		if (!format) 
			return;

		va_list arg;

		StringBuffer64 log_level;
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
//{
//#if TWEAK_ENABLE_LOGGING
//	if (priority <= LogLevel)
//	{
//		va_list arg;
//		va_start(arg, format);
//
//		char buffer[512];
//		vsnprintf(buffer, sizeof(buffer), format, arg);
//		va_end(arg);
//
//		StringBuffer64 log_level;
//		PriorityLevelToText(priority, log_level);
//
//		char final[512];
//		if (!log_level.empty())
//			snprintf(final, sizeof(final), "%s: %s\n", log_level.c_str(), buffer);
//		else
//			snprintf(final, sizeof(final), "%s\n", buffer);
//
//		OutputDebugStringA(final);
//	}
//#endif
//}

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

		StringBuffer64 log_level;
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