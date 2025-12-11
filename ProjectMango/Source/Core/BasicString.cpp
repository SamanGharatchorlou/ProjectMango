#include "pch.h"
#include "BasicString.h"

#include <cstring>

BasicString::BasicString(const char* string) : mBuffer(nullptr), mLength(0), mCap(0)
{
	if (string)
	{
		mLength = (uint32_t)strlen(string);
		mCap = mLength + 1;
		mBuffer = new char[mCap];
		assignTerminated(string);
	}
}

BasicString::BasicString(const BasicString& string) : BasicString(string.c_str()) { }


BasicString::BasicString(const char* string, unsigned int length)
{
	mLength = length; //strlen(string);
	mCap = length + 1;
	mBuffer = new char[mCap];

	memcpy(mBuffer, string, mLength + 1);
}


const BasicString BasicString::Ref(const char* string)
{
	BasicString reference_string;
	reference_string.mLength = (uint32_t)strlen(string);
	reference_string.mCap = reference_string.mLength + 1;
	reference_string.mBuffer = (char*)string;

	reference_string.ownsBuffer = false;

	return reference_string;
}


BasicString::BasicString(int number)
{
	char buf[32];
    _itoa(number, buf, 10);

	mLength = (uint32_t)strlen(buf);
	mCap = mLength + 1;
	mBuffer = new char[mCap];
	assignTerminated(buf);
}

//BasicString::BasicString(float number)
//{
//	char tempBuffer[20];
//	sprintf_s(tempBuffer, "%.f", number);
//
//	mLength = (uint32_t)strlen(tempBuffer);
//	mCap = mLength + 1;
//
//	mBuffer = new char[mCap];
//	memcpy(mBuffer, tempBuffer, mCap);
//}
//
//
//BasicString::BasicString(float number, int precision)
//{
//	char tempBuffer[20];
//
//	char formatBuffer[5] = "%.";
//	_itoa(precision, formatBuffer + 2, 10);
//	strcat(formatBuffer, "f\0");
//
//	sprintf_s(tempBuffer, formatBuffer, number);
//
//	mLength = (uint32_t)strlen(tempBuffer);
//	mCap = mLength + 1;
//
//	mBuffer = new char[mCap];
//	memcpy(mBuffer, tempBuffer, mCap);
//}

BasicString::~BasicString()
{
	if(ownsBuffer)
		delete[] mBuffer;

	eliminate();
}

void BasicString::eliminate()
{
	mLength = 0;
	mCap = 0;
	mBuffer = nullptr;
}

//void BasicString::set(const char* string)
//{
//	uint32_t strLength = (uint32_t)strlen(string);
//
//	if (strLength < mCap)
//	{
//		assignTerminated(string);
//		mBuffer[strLength + 1] = '\0';
//	}
//	else
//	{
//		resizeBuffer(strLength + 1);
//		set(string);
//	}
//}

void BasicString::SetLength(int length)
{
	if(!ownsBuffer)
		return;

	if (length >= mCap)
	{
		resizeBuffer(length);
	}
	else if(length < mCap)
	{
		mLength = length;
		mBuffer[mLength] = '\0';
	}
}

BasicString BasicString::substr(int start, int length) const
{
	const char* backEndString = &mBuffer[start];
	BasicString subbedStr(backEndString, length);
	return subbedStr;
}

BasicString& BasicString::concat(const char* string)
{
	if(!ownsBuffer)
		return *this;

	const uint32_t str_len = (uint32_t)strlen(string);
	if ((mLength + str_len) < mCap)
	{
		strcat_s(mBuffer, mCap, string);
		mLength = (uint32_t)strlen(mBuffer);
		return *this;
	}
	else
	{
		resizeBuffer(mLength + str_len + 1);
		return concat(string);
	}
}

void BasicString::clear()
{
	memset(mBuffer, 0, mLength);
	mLength = 0;
}

const char* BasicString::FindSubString(const char* subString) const
{
	if(mBuffer)
		return strstr(mBuffer, subString);

	return nullptr;
}


void BasicString::getInput(const BasicString& message)
{
	printf("%s", message.c_str());
	
	clear();

	std::cin.get(mBuffer, mCap);
	std::cin.ignore();

	calculateLength();
}


// --- Private Functions --- //
void BasicString::assignTerminated(const char* string)
{
	if(!ownsBuffer)
		return;

	// i think i need this, copying an empty string breaks the code
	if(string)
	{
		mLength = (uint32_t)strlen(string);
		memcpy(mBuffer, string, mLength + 1);
	}
}


void BasicString::setNewBuffer(int size)
{
	if(!ownsBuffer)
		return;

	delete[] mBuffer;

	eliminate();

	if (size > 0)
	{
		mCap = size + 1;
		mBuffer = new char[mCap];
	}
}


void BasicString::resizeBuffer(int size)
{
	if(!ownsBuffer)
		return;

	char* new_buffer = nullptr;

	mCap = size;
	if (mCap > 0)
	{
		new_buffer = new char[mCap];
		mLength = Maths::Min(mCap - 1, mLength);

		if (mLength > 0)
		{
			memcpy(new_buffer, mBuffer, mLength);
			new_buffer[mLength] = '\0';
		}
	}

	delete[] mBuffer;
	mBuffer = new_buffer;
}


// Operator overloads
BasicString& BasicString::operator = (const char* string)
{
	uint32_t length = (uint32_t)strlen(string);
	if (length > 0)
	{
		if (length >= mCap)
			setNewBuffer(length);

		assignTerminated(string);
	}
	return *this;
}

BasicString& BasicString::operator = (const BasicString& basicString)
{
	unsigned int length = basicString.length();
	if (length > 0)
	{
		if (length >= mCap)
			setNewBuffer(length);

		assignTerminated(basicString.c_str());
	}
	return *this;
}


// TODO: is basicStringA is null then this breaks...
bool operator == (const BasicString& basicString, const char* string)
{
	return strncmp(basicString.c_str(), string, basicString.length() + 1) == 0;
}
bool operator == (const BasicString& basicStringA, const BasicString& basicStringB)
{
	if(basicStringA.buffer() == nullptr || basicStringB.buffer() == nullptr )
		return basicStringA.buffer() == basicStringB.buffer();

	return strncmp(basicStringA.c_str(), basicStringB.c_str(), basicStringA.length() + 1) == 0;
}


BasicString operator + (BasicString basicString, const char* string)
{
	return basicString.concat(string);
}
BasicString operator + (BasicString basicStringA, const BasicString& basicStringB)
{
	return basicStringA.concat(basicStringB.c_str());
}


