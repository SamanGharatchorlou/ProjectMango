#pragma once

//class XMLNode;

//class StringMap32 //: public Map<StringBuffer32, StringBuffer32>
//{
//public:
//	StringMap32() { }
//
//	//void fillAtributes(const XMLNode& node);
//	//void fillValues(const XMLNode& node);
//
//	StringBuffer32 at(const char* key) const { return mData.at(key); }
//	bool contains(const char* key) const { return mData.count(key) > 0; }
//
//	const char* getString(const char* key) const;
//	bool getBool(const char* key) const;
//	float getFloat(const char* key) const;
//	int getInt(const char* key) const;
//	VectorF getVectorF(const char* x, const char* y) const;
//	Vector2D<int> getVectorI(const char* x, const char* y) const;
//
//	std::unordered_map<StringBuffer32, StringBuffer32> mData;
//};

template<class T>
struct SettingValues
{
	inline T operator [] (const char* label) const { return data.at(label); }
	inline T& operator [] (const char* label) { return data[label]; }

	std::unordered_map<StringBuffer32, T> data;
};

//struct SettingStrings
//{
//	inline const char* operator [] (const char* label) const { return data.at(label).c_str(); }
//	inline BasicString& operator [] (const char* label) { return data[label]; }
//
//	std::unordered_map<StringBuffer32, BasicString> data;
//};

template<class T>
struct SettingArrays
{
	inline std::vector<T> operator [] (const char* label) const { return data.at(label); }
	inline std::vector<T>& operator [] (const char* label) { return data[label]; }

	std::unordered_map<StringBuffer32, std::vector<T>> data;
};

struct Settings
{
	inline float GetFloat(const char* label, float default_value = 0) const
	{
		if(values.data.contains(label))
			return values.data.at(label);

		return default_value;
	}

	inline int GetInt(const char* label, int default_value = 0) const
	{
		if(values.data.contains(label))
			return (int)values.data.at(label);

		return default_value;
	}

	inline float GetBool(const char* label, bool default_value = false) const
	{
		if(values.data.contains(label))
			return (bool)values.data.at(label);

		return default_value;
	}
		
	inline const char* GetString(const char* label, const char* default_value = nullptr) const
	{
		if (strings.data.contains(label))
			return strings.data.at(label).c_str();

		return default_value;
	}

	VectorF GetVectorF(const char* x, const char* y) const;
	VectorF GetVectorF(const char* label) const; // adds _x and _y to the label
	
	inline bool Contains(const char* key) const { return values.data.contains(key) || strings.data.contains(key); }

	SettingValues<BasicString> strings;
	SettingValues<float> values;

	SettingArrays<float> floatArrays;
};