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
	//inline std::vector<T> operator [] (const char* label) const { return data.at(label); }
	//inline std::vector<T>& operator [] (const char* label) { return data[label]; }

	std::unordered_map<StringBuffer32, std::vector<T>> data;
};

struct Settings
{
	// float
	inline float GetFloat(const char* label, float default_value = 0) const
	{
		if(floats.data.contains(label))
			return floats.data.at(label);

		return default_value;
	}	
	inline void AddFloat(const char* label, float value)
	{
		floats.data[label] = value;
	}

	// int
	inline int GetInt(const char* label, int default_value = 0) const
	{
		if(ints.data.contains(label))
			return (int)ints.data.at(label);

		return default_value;
	}	
	inline void AddInt(const char* label, int value)
	{
		ints.data[label] = (u64)value;
	}

	// u64
	inline u64 GetU64(const char* label, u64 default_value = 0) const
	{
		if (ints.data.contains(label))
			return ints.data.at(label);

		return default_value;
	}	
	inline void AddU64(const char* label, u64 value)
	{
		ints.data[label] = value;
	}

	// bool
	inline float GetBool(const char* label, bool default_value = false) const
	{
		if(ints.data.contains(label))
			return (bool)ints.data.at(label);

		return default_value;
	}	
	inline void AddBool(const char* label, bool value)
	{
		ints.data[label] = (u64)value;
	}
		
	// string
	inline const char* GetString(const char* label, const char* default_value = nullptr) const
	{
		if (strings.data.contains(label))
			return strings.data.at(label).c_str();

		return default_value;
	}	
	inline void AddString(const char* label, const char* value)
	{
		strings.data[label] = value;
	}

	// float array
	inline VectorF GetVector(const char* label, VectorF default_value = VectorF(0,0) ) const
	{
		if (floatArrays.data.contains(label))
		{
			const std::vector<float>& float_array = floatArrays.data.at(label);
			return VectorF(float_array[0], float_array[1]);
		}

		return default_value;
	}	
	inline void AddVectorF(const char* label, const VectorF& value)
	{
		//std::vector<float> vector = { value.x, value.y };
		floatArrays.data[label] = { value.x, value.y };
	}
	inline const std::vector<float>* GetFloatArray(const char* label) const
	{
		if (floatArrays.data.contains(label))
			return &floatArrays.data.at(label);

		return nullptr;
	}
	inline void AddFloatArray(const char* label, const std::vector<float>& value)
	{
		floatArrays.data[label] = value;
	}

	// int array
	inline const std::vector<u64>* GetIntArray(const char* label) const
	{
		if (intArrays.data.contains(label))
			return &intArrays.data.at(label);

		return nullptr;
	}
	inline void AddIntArray(const char* label, const std::vector<u64>& value)
	{
		intArrays.data[label] = value;
	}	
	inline void AddIntArrayMember(const char* label, u64 value)
	{
		intArrays.data[label].push_back(value);
	}

	inline SColour GetColour(const char* label, SColour default_value = SColour() ) const
	{
		if (strings.data.contains(label))
		{
			int hex = 0;
			const char* string = strings.data.at(label).c_str();
			std::stringstream ss(string + 1);
			ss >> std::hex >> hex;

			return SColour(hex);
		}

		return default_value;
	}

	// this has become prerry mega
	inline bool Contains(const char* key) const { 
		return floats.data.contains(key) ||
			ints.data.contains(key) ||
			strings.data.contains(key) ||
			//vectors.data.contains(key) ||
			floatArrays.data.contains(key) ||
			intArrays.data.contains(key);
	}

	void Merge(const Settings& other)
	{
		for (auto& [key, val] : other.strings.data)
			strings.data[key] = val;

		for (auto& [key, val] : other.floats.data)
			floats.data[key] = val;

		for (auto& [key, val] : other.ints.data)
			ints.data[key] = val;

		//for (auto& [key, val] : other.vectors.data)
		//	vectors.data[key] = val;

		for (auto& [key, val] : other.floatArrays.data)
			floatArrays.data[key] = val;

		for (auto& [key, val] : other.intArrays.data)
			intArrays.data[key] = val;
	}

	// basic types
	SettingValues<BasicString> strings;
	SettingValues<float> floats;
	SettingValues<u64> ints;

	//SettingValues<VectorF> vectors;

	// arrays
	SettingArrays<float> floatArrays;
	SettingArrays<u64> intArrays;
};