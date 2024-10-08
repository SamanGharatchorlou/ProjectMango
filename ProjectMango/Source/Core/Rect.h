#pragma once

#include "Vector2D.h"
#include "Debugging/Logging.h"

template<class T>
class Rect
{
public:

	// Constructors
	Rect() { SetRect(0, 0, 0, 0); }
	Rect(T x1, T y1, T x2, T y2) { SetRect(x1, y1, x2, y2); }
	Rect(Vector2D<T> top_left, Vector2D<T> size) { SetRect(top_left, size); }
	//Rect(int isValid) { if(isValid == -1) SetRect(-1, -1, -1, -1); }

	template<class K>
	Rect(const Rect<K>& rect) { x1 = (T)rect.x1; x2 = (T)rect.x2; y1 = (T)rect.y1; y2 = (T)rect.y2; }

	// Initialisations
	inline void SetRect(T left, T top, T right, T bot) { x1 = left; y1 = top; x2 = right, y2 = bot; }
	inline void SetRect(Vector2D<T> top_left, Vector2D<T> dimentions);
	inline void SetRect(const Rect<T>& rect) { x1 = rect.x1; y1 = rect.y1; x2 = rect.x2; y2 = rect.y2; }

	// Corners
	inline Vector2D<T> TopLeft() const { return Vector2D<T>(x1, y1); }
	inline Vector2D<T> TopRight() const { return Vector2D<T>(x2, y1); }
	inline Vector2D<T> BotRight() const { return Vector2D<T>(x2, y2); }
	inline Vector2D<T> BotLeft() const { return Vector2D<T>(x1, y2); }


	// Side Midpoints
	inline Vector2D<T> TopCenter() const { return Vector2D<T>(x1 + Width() / (T)2, y1); }
	inline Vector2D<T> BotCenter() const { return Vector2D<T>(x1 + Width() / (T)2, y2); }
	inline Vector2D<T> LeftCenter() const { return Vector2D<T>(x1, y1 + Height() / (T)2); }
	inline Vector2D<T> RightCenter() const { return Vector2D<T>(x2, y1 + Height() / (T)2); }


	// Sides - returns a new rect with zero width or height
	inline Rect<T> TopSide() const { return Rect<T>(x1, x2, y1, y1); }
	inline Rect<T> RightSide() const { return Rect<T>(x2, x2, y1, y1); }
	inline Rect<T> BotSide() const { return Rect<T>(x1, x2, y1, y1); }
	inline Rect<T> LeftSide() const { return Rect<T>(x1, x2, y1, y1); }


	// Side Points
	inline T TopPoint() const { return y1; }
	inline T RightPoint() const { return x2; }
	inline T BotPoint() const { return y2; }
	inline T LeftPoint() const { return x1; }


	// Centers                        
	inline Vector2D<T> Center() const { return Vector2D<T>(HCenter(), VCenter()); }
	inline T HCenter() const { return (x1 + x2) / (T)2; }
	inline T VCenter() const { return (y1 + y2) / (T)2; }

	// Set Dimentions
	inline void SetSize(Vector2D<T> size) 
	{
		ASSERT(size.x >= 0 && size.y >= 0, "Negative rect size, invalid");
		x2 = x1 + size.x; y2 = y1 + size.y; 
	}
	inline void SetSize(T x, T y) 
	{ 
		ASSERT(x >= 0 && y >= 0, "Negative rect size, invalid");
		x2 = x1 + x; y2 = y1 + y; 
	}

	inline void SetWidth(T width) 
	{ 
		ASSERT(width >= 0, "Negative rect width, invalid");
		x2 = x1 + width; 
	}
	inline void SetHeight(T height) 
	{
		ASSERT(height >= 0, "Negative rect height, invalid");
		y2 = y1 + height; 
	}

	inline void Scale(Vector2D<T> scale);

	// Get Dimentions
	inline const T Width() const { return x2 - x1; }
	inline const T Height() const { return y2 - y1; }


	// Set Position
	inline void SetTopLeft(Vector2D<T> point);
	inline void SetBotRight(Vector2D<T> point);
	inline void SetBotLeft(Vector2D<T> point);
	inline void SetTopRight(Vector2D<T> point);
	inline void SetTopCenter(Vector2D<T> point);
	inline void SetBotCenter(Vector2D<T> point);
	inline void SetLeftCenter(Vector2D<T> point);
	inline void SetRightCenter(Vector2D<T> point);
	inline void SetCenter(T x, T y);
	inline void SetCenter(Vector2D<T> point);


	// Movement
	inline void Translate(Vector2D<T> vector);
	//inline Rect<T> Translate(T lateral, T vertical) const { return Rect<T>(x1 + lateral, y1 + vertical, x2 + lateral, y2 + vertical); }
	inline Rect<T> MoveCopy(Vector2D<T> vector) const { return Rect<T>(TopLeft() + vector, Size()); }

	// Get Size
	inline Vector2D<T> Size() const { return Vector2D<T>(Width(), Height()); }


	// Zero rect
	inline void Zero() { x1 = x2 = y1 = y2 = 0; }


	// Validity 
	inline bool isValid() const { return (x2 > x1 && y2 > y1 ); }


	// return true if rectangles overlap
	bool Intersect(const Rect<T>& rect) const;
	Vector2D<T> closestRectSide(Vector2D<T> point) const;

	// SDL
	SDL_Rect toSDLRect() const;


	// -- Operators --
	inline Rect<T> operator * (T scalar) { return Rect<T>(x1, y1, x1 + (Width() * scalar), y1 + (Height() * scalar)); }
	inline Rect<T> operator + (const Rect<T>& rect) { return Rect<T>(x1 + rect.x1, y1 + rect.y1, x2 + rect.x2, y2 + rect.y2); }

#if DEBUG_MODE
	BasicString  infoString();
#endif

	// Corner members
	T x1;
	T x2;
	T y1;
	T y2;
};

// typedefs
typedef Rect<float>  RectF;
#define InvalidRectF Rect<float>(0.0f, -1.0f, 0.0f, - 1.0f)

template <class T>
inline void Rect<T>::SetRect(const Vector2D<T> top_left, const Vector2D<T> dimentions)
{
	x1 = top_left.x;
	x2 = top_left.x + dimentions.x;

	y1 = top_left.y;
	y2 = top_left.y + dimentions.y;
}

// Returns 1 if the rectangles overlap
template <class T>
inline bool Rect<T>::Intersect(const Rect<T> &rect) const
{
	if ((y2 >= rect.y1) && (rect.y2 >= y1))
		if ((rect.x2 >= x1) && (x2 >= rect.x1))
			return 1;
	return 0;
}


template <class T>
inline void Rect<T>::SetTopLeft(Vector2D<T> point)
{ 
	T width = Width();
	T height = Height();

	x1 = point.x; 
	x2 = point.x + width; 
	y1 = point.y; 
	y2 = point.y + height;
}


template <class T>
inline void Rect<T>::SetTopRight(Vector2D<T> point)
{
	T width = Width();
	T height = Height();

	x1 = point.x - width; 
	x2 = point.x; 
	y1 = point.y; 
	y2 = point.y + height;
}


template <class T>
inline void Rect<T>::SetBotRight(Vector2D<T> point)
{
	T width = Width();
	T height = Height();

	x1 = point.x - width;
	x2 = point.x;
	y1 = point.y - height;
	y2 = point.y;
}

template <class T>
inline void Rect<T>::SetBotLeft(Vector2D<T> point)
{
	T width = Width();
	T height = Height();

	x1 = point.x;
	x2 = point.x + width;
	y1 = point.y - height;
	y2 = point.y;
}


template <class T>
inline void Rect<T>::SetLeftCenter(Vector2D<T> point)
{
	T halfWidth = Width() / 2;
	T halfHeight = Height() / 2;

	x1 = point.x;
	x2 = point.x + halfWidth * 2;
	y1 = point.y - halfHeight;
	y2 = point.y + halfHeight;
}


template <class T>
inline void Rect<T>::SetRightCenter(Vector2D<T> point)
{
	T halfWidth = Width() / 2;
	T halfHeight = Height() / 2;

	x1 = point.x - halfWidth * 2;
	x2 = point.x;
	y1 = point.y - halfHeight;
	y2 = point.y + halfHeight;
}


template <class T>
inline void Rect<T>::SetTopCenter(Vector2D<T> point)
{
	T halfWidth = Width() / 2;
	T height = Height();

	x1 = point.x - halfWidth;
	x2 = point.x + halfWidth;
	y1 = point.y;
	y2 = point.y + height;
}

template <class T>
inline void Rect<T>::SetBotCenter(Vector2D<T> point)
{
	T halfWidth = Width() / 2;
	T halfHeight = Height() / 2;

	x1 = point.x - halfWidth;
	x2 = point.x + halfWidth;
	y1 = point.y - halfHeight * 2;
	y2 = point.y;
}


template <class T>
inline void Rect<T>::SetCenter(Vector2D<T> point)
{
	T halfWidth = Width() / 2;
	T halfHeight = Height() / 2;

	x1 = point.x - halfWidth;
	x2 = point.x + halfWidth;
	y1 = point.y - halfHeight;
	y2 = point.y + halfHeight;
}

template <class T>
inline void Rect<T>::SetCenter(T x, T y)
{
	T halfWidth = Width() / 2;
	T halfHeight = Height() / 2;

	x1 = x - halfWidth;
	x2 = x + halfWidth;
	y1 = y - halfHeight;
	y2 = y + halfHeight;
}

template <class T>
inline void Rect<T>::Scale(Vector2D<T> scale)
{
	SetTopLeft(TopLeft() * scale);
	SetSize(Size() * scale);
}


template <class T>
inline void Rect<T>::Translate(Vector2D<T> vector)
{
	x1 += vector.x;
	x2 += vector.x;
	y1 += vector.y;
	y2 += vector.y;
}

template <class T>
SDL_Rect Rect<T>::toSDLRect() const
{
	return SDL_Rect{ static_cast<int>(x1 + 0.5f), static_cast<int>(y1 + 0.5f),
				static_cast<int>(Width() + 0.5f), static_cast<int>(Height() + 0.5f) };
}

template <class T>
Vector2D<T> Rect<T>::closestRectSide(Vector2D<T> point) const
{
	VectorF rectSides[] = { TopCenter(), RightCenter(), BotCenter(), LeftCenter() };

	int nearestSide = -1;
	float leastDistance = FLT_MAX;

	for (int i = 0; i < 4; i++)
	{
		const float distance = distanceSquared(point, rectSides[i]);
		if (distance < leastDistance)
		{
			nearestSide = i;
			leastDistance = distance;
		}
	}

	return rectSides[nearestSide];
}


#if DEBUG_MODE
template <class T>
BasicString Rect<T>::infoString()
{
	const int len = 512;
	char buffer[len];
	const char* format = "// ------- rect info string ------- \n\
							top left (%.f, %.f) \n\
							width, height (%.f, %.f) \n";

	snprintf_s(buffer, len, format, x1, y1, Width(), Height());
	return BasicString(buffer);
}
#endif