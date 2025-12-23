#pragma once

template<class T>
struct SettingValues
{
	inline T operator [] (const char* label) const { return data.at(label); }
	inline T& operator [] (const char* label) { return data[label]; }

	std::unordered_map<StringBuffer32, T> data;
};

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

	inline VectorF GetVector(const char* label, VectorF default_value = VectorF(0,0) ) const
	{
		if(floatArrays.data.contains(label))
		{
			const std::vector<float>& array = floatArrays.data.at(label);
			return VectorF(array[0], array[1]);
		}

		return default_value;
	}

	inline bool Contains(const char* key) const { return values.data.contains(key) || strings.data.contains(key) || floatArrays.data.contains(key); }

	SettingValues<BasicString> strings;
	SettingValues<float> values;

	SettingArrays<float> floatArrays;
};