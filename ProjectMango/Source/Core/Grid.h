#pragma once

#include "Vector2D.h"
#include "Debugging/Logging.h"

template<class T>
class Grid
{
public:
	Grid() { }
	Grid(u32 x, u32 y, T value);

	std::vector<T>& operator [] (int y);
	T& operator [] (Vector2D<int> index);
	T& get(int x, int y) { return data[y][x]; }

	void clear();
	void set(Vector2D<int> size, T value);
	void setAllValues(T value);

	const T& get(VectorI index) const { return data[index.y][index.x]; }
	const std::vector<std::vector<T>>& get() const { return data; }

	const u32 rows() const { return (u32)data.size(); }
	const u32 colums() const { return (u32)data[0].size(); }
	const u32 yCount() const { return (u32)data.size(); }
	const u32 xCount() const { return (u32)data[0].size(); }
	const VectorI size() const { return VectorI(colums(), rows()); }

	bool inBounds(Vector2D<int> index) const;


private:
	// should i new this?
	std::vector<std::vector<T>> data;
};


template<class T>
Grid<T>::Grid(u32 x, u32 y, T value)
{
	ASSERT(y > 0 && x > 0,
		"Grid must have at least 1 column and 1 row, set with %d columns, %d rows", y, x);

	std::vector<T> row(x, value);
	data = std::vector<std::vector<T>>(y, row);
}

template<class T>
std::vector<T>& Grid<T>::operator [] (int y)
{
#if DEBUG_MODE
	if (y >= 0 && y < yCount() == false)
		DebugPrint(Error, "Attempting to get out of bounds row %d. Data only has %d rows", y, yCount());
#endif

	ASSERT(y >= 0 && y < yCount(),
		"Attempting to get out of bounds row %d. Data only has %d rows", y, yCount());

	return data[y];
}

template<class T>
T& Grid<T>::operator [] (Vector2D<int> index)
{
	ASSERT(inBounds(index),
		"Attempting to get out of bounds index %d, %d in data of size %d, %d ",
		index.x, index.y, xCount(), yCount());

	return data[index.y][index.x];
}


template<class T>
void Grid<T>::set(Vector2D<int> size, T value)
{
	ASSERT(size.y > 0 && size.x > 0,
		"Grid must have at least 1 column and 1 row, set with %d columns, %d rows", size.y, size.x);

	std::vector<T> row(size.x, value);
	data = std::vector<std::vector<T>>(size.y, row);
}


template<class T>
void Grid<T>::setAllValues(T value)
{
	for (int i = 0; i < data.size(); i++)
	{
		std::fill(data[i].begin(), data[i].end(), value);
	}
}



template<class T>
void Grid<T>::clear()
{
	data.clear();
}


template<class T>
bool Grid<T>::inBounds(Vector2D<int> index) const
{
	return index.x >= 0 && index.x < xCount() &&
		index.y >= 0 && index.y < yCount();
}
