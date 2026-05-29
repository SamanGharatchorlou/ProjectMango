#pragma once

#include "Vector2D.h"


//--------------------------------------------------------
// Bitwise helpers
static bool HasFlag(uint32_t flag, uint32_t marker)
{
	return flag & marker;
}

template<class T>
static void SetFlag(T& flag, T marker)
{
	flag |= marker;
}

template<class T>
static void RemoveFlag(T& flag, T marker)
{
	flag &= ~marker;
}


//--------------------------------------------------------
// Vector helpers
template<typename T>
void merge(std::vector<T>& vectorA, const std::vector<T>& vectorB)
{
	vectorA.insert(vectorA.end(), vectorB.begin(), vectorB.end());
}

template<typename T>
bool Contains(std::vector<T>& vector, const T& item)
{
	for( u32 i = 0; i < vector.size(); i++ )
	{
		if(vector[i] == item)
			return true;
	}

	return false;
}

template<typename T>
bool PushBackUnique(std::vector<T>& vector, const T& item)
{
	if( !Contains(vector, item) )
	{
		vector.push_back(item);
		return true;
	}

	return false;
}

template<typename T>
bool EraseSwap(std::vector<T>& vector, const T& item)
{
	for( auto iter = vector.begin(); iter != vector.end(); iter++ )
	{
		if(*iter == item)
		{
			*iter = vector.back();
			vector.resize(vector.size() - 1);
			return true;
		}
	}

	return false;
}

template<typename T>
void Erase(std::vector<T>& vec, const T& item)
{
	vec.erase(std::remove(vec.begin(), vec.end(), item), vec.end());
}

static float DotProduct(VectorF a, VectorF b)
{
	return a.x * b.x + a.y * b.y;
}

static float GetAngle(VectorF a, VectorF b)
{
	float dot = DotProduct(a, b);
	float mag = a.length() * b.length();
	return (float)acos(dot / mag);
}


//--------------------------------------------------------
// Maths helpers
template<typename T>
T Power(T value, int count )
{
	T out_value = value;
	for( int i = 0; i < count - 1; i++ )
	{
		out_value *= out_value;
	}

	return out_value;
}

float EaseOut(float value, int easing_factor);


//--------------------------------------------------------
// String helpers
static bool toBool(const char* string)
{
	return StringCompare(string, "true", 4);
}

static int toInt(const char* string)
{
	return atoi(string);
}

static float toFloat(const char* string)
{
	return (float)atof(string);
}

static VectorF toVectorF(const char* x, const char* y)
{
	float xVec = toFloat(x);
	float yVec = toFloat(y);
	return VectorF(xVec, yVec);
}

static Vector2D<int> toVectorI(const char* x, const char* y)
{
	int xVec = toInt(x);
	int yVec = toInt(y);
	return Vector2D<int>(xVec, yVec);
}

//--------------------------------------------------------
// map helpers
template<class T, class K>
static void log(const std::unordered_map<T,K>& map)
{
	DebugPrint(Log, "\n-String Data Map Values-");
	for (auto iter = map.begin(); iter != map.end(); iter++)
	{
		DebugPrint(Log, "Data map %s has value %s", iter->first.c_str(), iter->second.c_str());
	}
	DebugPrint(Log, "----------------");
}

template <class T, class K>
static void merge(std::unordered_map<T, K>& map_a, std::unordered_map<T, K>& map_b)
{
	for (auto iter = map_b.begin(); iter != map_b.end(); iter++)
	{
		map_a[iter->first] = iter->second;
	}
}


////--------------------------------------------------------
//// rect helpers
//bool PointInRect(RectF rect, VectorF position) 
//{
//	return !(	position.x > rect.RightPoint() || 
//				position.x < rect.LeftPoint()  || 
//				position.y > rect.BotPoint()   || 
//				position.y < rect.TopPoint());
//}

static VectorI IndexToGrid(int index, int grid_width)
{
	int index_y = (int)((float)index / grid_width);
	int index_x = (int)(index - (index_y * grid_width));
	return VectorI(index_x, index_y);
}

static bool Contains(const RectF& rect, VectorF point)
{
	return !(point.x > rect.RightPoint() ||
				point.x < rect.LeftPoint() ||
				point.y > rect.BotPoint() ||
				point.y < rect.TopPoint());
}


//--------------------------------------------------------
// Identifiers
bool IsPlayer(ECS::Entity entity);
bool IsEnemy(ECS::Entity entity);
bool IsTerrain(ECS::Entity entity);

bool IsTargetInFrontOfSource(ECS::Entity target, ECS::Entity);

bool IsSelectedDebugEntity(ECS::Entity entity);

float AdjustToScreenSize(float size);
VectorF AdjustToScreenSize(VectorF size);
float RevertFromScreenSize(float size);