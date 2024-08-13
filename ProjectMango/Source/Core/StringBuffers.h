#pragma once

#if DEBUG_CHECK
#include "Debugging/Logging.h"
#endif

static inline bool StringCompare(const char* string_a, const char* string_b, uint32_t len)
{
	return strncmp(string_a, string_b, len) == 0;
}

static inline bool StringCompare(const char* string_a, const char* string_b)
{
	return strcmp(string_a, string_b) == 0;
}

class StringBuffer32
{
public:
	StringBuffer32() { clear(); }
	StringBuffer32(const char* string) { set(string); }

	static constexpr uint32_t bufferLength() { return 32; }
	char* buffer() { return mBuffer; }
	const char* c_str() const { return mBuffer; }


	void set(const char* string) 
	{
#if DEBUG_CHECK
		if (strlen(string) > bufferLength())
			DebugPrint(Error, "Attempting to set StringBuffer%d with string of size %d, will drop overflow characters", bufferLength(), strlen(string));
#endif
		strncpy(mBuffer, string, bufferLength()); 
	}

	int length() const { return strlen(mBuffer); }
	bool empty() const { return strlen(mBuffer) == 0; }
	void clear() { memset(mBuffer, 0, bufferLength()); }

	StringBuffer32 operator + (const StringBuffer32& string) const
	{
		StringBuffer32 new_string;
		snprintf(new_string.buffer(), new_string.bufferLength(), "%s%s", mBuffer, string.c_str());
		return new_string;
	}

	void operator = (const StringBuffer32& string)
	{
		set(string.c_str());
	}
	void operator = (const char* string)
	{
		set(string);
	}

	bool operator == (const StringBuffer32& string) const
	{
		return StringCompare(mBuffer, string.c_str(), bufferLength());
	}
	bool operator == (const char* string) const
	{
		return StringCompare(mBuffer, string, bufferLength());
	}


private:
	char mBuffer[32];
};

class StringBuffer64
{
public:
	StringBuffer64() { memset(mBuffer, 0, bufferLength()); }
	StringBuffer64(const char* string) { set(string); }

	static constexpr uint32_t bufferLength() { return 64; }
	char* buffer() { return mBuffer; }
	const char* c_str() const { return mBuffer; }

	void set(const char* string) 
	{
#if DEBUG_CHECK
		if (strlen(string) > bufferLength())
			DebugPrint(Error, "Attempting to set StringBuffer%d with string of size %d, will drop overflow characters", bufferLength(), strlen(string));
#endif
		strncpy(mBuffer, string, bufferLength()); 
	}
	int length() const { return strlen(mBuffer); }
	bool empty() const { return strlen(mBuffer) == 0; }

	StringBuffer64 operator + (const StringBuffer64& string)
	{
		StringBuffer64 new_string;
		snprintf(new_string.buffer(), new_string.bufferLength(), "%s%s", mBuffer, string.c_str());
		return new_string;
	}

	void operator = (const StringBuffer32& string)
	{
		set(string.c_str());
	}
	void operator = (const char* string)
	{
		set(string);
	}

	bool operator == (const StringBuffer64& string) const
	{
		return StringCompare(mBuffer, string.c_str(), bufferLength());
	}
	bool operator == (const char* string) const
	{
		return StringCompare(mBuffer, string, bufferLength());
	}

private:
	char mBuffer[64];
};


/*
Define BasicString hash for use in maps
djb2 hash function by Dan Bernstein. http://www.cse.yorku.ca/~oz/hash.html.
*/
namespace std
{
	template <>
	struct hash<StringBuffer32>
	{
		std::size_t operator()(const StringBuffer32& string) const
		{
			const char* str = string.c_str();
			unsigned long hash = 5381;
			unsigned int c;

			while (c = *str++)
				hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

			return hash;
		}
	};

	template <>
	struct hash<StringBuffer64>
	{
		std::size_t operator()(const StringBuffer64& string) const
		{
			const char* str = string.c_str();
			unsigned long hash = 5381;
			unsigned int c;

			while (c = *str++)
				hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

			return hash;
		}
	};
}


//bool operator==(const char* const lhs, const char* const rhs)
//{
//	return StringCompare(lhs, rhs, 5);
//}