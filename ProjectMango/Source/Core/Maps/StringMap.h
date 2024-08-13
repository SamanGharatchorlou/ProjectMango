#pragma once

//#include "Map.h"

class XMLNode;

class StringMap32 //: public Map<StringBuffer32, StringBuffer32>
{
public:
	StringMap32() { }

	void fillAtributes(const XMLNode& node);
	void fillValues(const XMLNode& node);

	StringBuffer32 at(const char* key) const { return mData.at(key); }
	bool contains(const char* key) const { return mData.count(key) > 0; }

	const char* getString(const char* key) const;
	bool getBool(const char* key) const;
	float getFloat(const char* key) const;
	int getInt(const char* key) const;
	VectorF getVectorF(const char* x, const char* y) const;
	Vector2D<int> getVectorI(const char* x, const char* y) const;

	std::unordered_map<StringBuffer32, StringBuffer32> mData;
};