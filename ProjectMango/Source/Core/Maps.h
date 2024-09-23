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


struct SettingValues
{
	inline float GetFloat(const char* label, float default_value = 0) const
	{
		if(data.contains(label))
			return data.at(label);

		return default_value;
	}

	inline float GetBool(const char* label, bool default_value = false) const
	{
		if(data.contains(label))
			return (bool)data.at(label);

		return default_value;
	}

	
	VectorF GetVectorF(const char* x, const char* y) const;

	inline bool Contains(const char* key) const { return data.contains(key); }
	
	inline float operator [] (const char* label) const { return data.at(label); }
	inline float& operator [] (const char* label) { return data[label]; }

	std::unordered_map<StringBuffer32, float> data;
};