#pragma once


class BasicString
{
public:
	BasicString() : mBuffer(nullptr), mLength(0), mCap(0) { }
	BasicString(const char* string);
	BasicString(const BasicString& string);
	BasicString(const char* string, unsigned int length);
	BasicString(int number);
	//BasicString(float number, int precision);

	// create a basic string without needing to allocate, can edit anything on it, since some one else owns it
	static const BasicString Ref(const char* string);

	~BasicString();

	const char* c_str() const { return mBuffer; }

	//void set(const char* string);
	void SetLength(int length);

	BasicString substr(int start, int length) const;

	uint32_t length() const { return (uint32_t)mLength; }
	uint32_t bufferLength() const { return mCap; }
	bool empty() const { return mLength == 0; }
	void calculateLength() { mLength = length(); }

	BasicString& concat(const char* string);

	void clear();
	void eliminate(); // Warning: does not delete buffer, but sets to nullptr

	char*& buffer() { return mBuffer; }
	const char* buffer() const { return mBuffer; }

	BasicString& operator = (const char* string);
	BasicString& operator = (const BasicString& string);

	const char* FindSubString(const char* subString) const;

	void getInput(const BasicString& message);

private:
	void setNewBuffer(int size);
	void resizeBuffer(int size);
	
	void assignTerminated(const char* string);


private:
	char* mBuffer = nullptr;
	uint32_t mLength = 0;
	uint32_t mCap = 0;

	bool ownsBuffer = true;
};


BasicString operator + (BasicString basicString, const char* string);
BasicString operator + (BasicString basicStringA, const BasicString& basicStringB);

bool operator == (const BasicString& basicString, const char* string);
bool operator == (const BasicString& basicStringA, const BasicString& basicStringB);


/* 
Define BasicString hash for use in maps
djb2 hash function by Dan Bernstein. http://www.cse.yorku.ca/~oz/hash.html.
*/
namespace std
{
	template <>
	struct hash<BasicString>
	{
		std::size_t operator()(const BasicString& string) const
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