#pragma once

#include "Maths.h"

template<class T>
class Vector2D
{
public:
	Vector2D() { x = 0; y = 0; }
	Vector2D(T _x, T _y) : x((T)_x), y((T)_y) { }

	// -- Functions -- //
	inline Vector2D<int> toInt() const { return Vector2D<int>((int)x, (int)y); }
	inline Vector2D<float> toFloat() const { return Vector2D<float>((float)x, (float)y); }
	static Vector2D<T> zero() { return Vector2D<T>(0, 0); }


	inline void set(T _x, T _y) { x = _x; y = _y; }
	inline bool isZero() const { return (x == 0.0f && y == 0.0f) ? true : false; } 
	inline bool isPositive() const { return (x > 0.0f && y > 0.0f) ? true : false; } 
	inline bool isNegative() const { return (x < 0.0f && y < 0.0f) ? true : false; } 
	inline bool hasNegative() const { return (x < 0.0f || y < 0.0f) ? true : false; }

	inline Vector2D<T> normalise();
	inline T lengthSquared() const { return (x * x) + (y * y); }
	inline T length() const { return std::sqrt((x * x) + (y * y)); }
	inline T area() const { return x * y; }
	inline T dot(Vector2D<T> vector) const { return(x * vector.x + y * vector.y); }

	static float distanceSquared(Vector2D<T> pointA, Vector2D<T> pointB);
	static float distance(Vector2D<T> pointA, Vector2D<T> pointB);

	// (0,1) = 0 degrees
	T getRotation() const;
	Vector2D<T> rotateVector(float dTheta, Vector2D<T> about) const;
	Vector2D<T> rotateVector(double sine, double cosine, Vector2D<T> about) const;

	Vector2D<T> clamp(const T min, const T max) const;
	Vector2D<T> clamp(const Vector2D<T> min, const Vector2D<T> max) const;

	Vector2D<T> realiseSize(T maxDimention) const;

	SDL_Point toSDLPoint();


	// -- Operators -- 
	inline Vector2D<T> operator + (const Vector2D<T> vector) const { return Vector2D<T>(x + vector.x, y + vector.y); }
	inline Vector2D<T> operator - (const Vector2D<T> vector) const { return Vector2D<T>(x - vector.x, y - vector.y); }
	inline Vector2D<T> operator + (const T scalar) const { return Vector2D<T>(x + scalar, y + scalar); }
	inline Vector2D<T> operator - (const T scalar) const { return Vector2D<T>(x - scalar, y - scalar); }

	inline Vector2D<T>& operator += (const Vector2D<T> vector) { x += vector.x; y += vector.y; return *this; }
	inline Vector2D<T>& operator -= (const Vector2D<T> vector) { x -= vector.x; y -= vector.y; return *this; }

	inline Vector2D<T> operator * (const Vector2D<T> vector) const { return Vector2D<T>(x * vector.x, y * vector.y); }
	inline Vector2D<T> operator / (const Vector2D<T> vector) const { return Vector2D<T>(x / vector.x, y / vector.y); }
	inline Vector2D<T> operator * (const T scalar) const { return Vector2D<T>(x * scalar, y * scalar); }
	inline Vector2D<T> operator / (const T scalar) const { return Vector2D<T>(x / scalar, y / scalar); }

	inline Vector2D<T>& operator *= (const Vector2D<T> vector) { x *= vector.x; y *= vector.y; return *this; }
	inline Vector2D<T>& operator *= (const T scalar) { x *= scalar; y *= scalar; return *this; }
	inline Vector2D<T>& operator /= (const T scalar) { x /= scalar; y /= scalar; return *this; }

	inline Vector2D<T>& operator = (const Vector2D<T> vector) { x = vector.x; y = vector.y; return *this; }
	inline bool operator == (const Vector2D<T> vector) const { return (x == vector.x && y == vector.y); }
	inline bool operator != (const Vector2D<T> vector) const { return (x != vector.x || y != vector.y); }


	// -- members --
	T x;
	T y;
};


// typedefs
typedef Vector2D<float>  VectorF;
typedef Vector2D<int>  VectorI;

static const VectorF c_invalidVector(FLT_MAX, FLT_MAX);

template<class T>
float Vector2D<T>::distanceSquared(Vector2D<T> pointA, Vector2D<T> pointB)
{
	return (float)((pointA.x - pointB.x) * (pointA.x - pointB.x) +
		(pointA.y - pointB.y) * (pointA.y - pointB.y));
}

template<class T>
float Vector2D<T>::distance(Vector2D<T> pointA, Vector2D<T> pointB)
{
	return (float)sqrt((pointA.x - pointB.x) * (pointA.x - pointB.x) +
		(pointA.y - pointB.y) * (pointA.y - pointB.y));
}

template<class T>
inline Vector2D<T> Vector2D<T>::normalise()
{
	Vector2D<T> normalised(x, y);

	if (x != 0 && y != 0)
	{
		T magnitude = std::sqrt((x * x) + (y * y));
		normalised = normalised / magnitude;
	}

	return normalised;
}

template<class T>
Vector2D<T> Vector2D<T>::rotateVector(float dTheta, Vector2D<T> about) const
{
	const double radians = Maths::toRadians(dTheta);
	const double cosine = cos(radians);
	const double sine = sin(radians);

	return rotateVector(sine, cosine, about);
}

template<class T>
Vector2D<T> Vector2D<T>::rotateVector(double sine, double cosine, Vector2D<T> about) const
{
	const Vector2D<T> originPoint = Vector2D<T>(x,y) - about;

	const double _x = (originPoint.x * cosine) - (originPoint.y * sine);
	const double _y = (originPoint.x * sine) + (originPoint.y * cosine);

	return Vector2D<T>((float)_x, (float)_y) + about;
}

// (0,1) = 0 degrees
template<class T>
T Vector2D<T>::getRotation() const
{
	return Maths::toDegrees(static_cast<T>(std::atan2(y, x)));
}

template<class T>
Vector2D<T> Vector2D<T>::clamp(const T min, const T max) const
{
	T _x = std::min(max, std::max(x, min));
	T _y = std::min(max, std::max(y, min));
	return Vector2D<T>(_x, _y);;
}

template<class T>
Vector2D<T> Vector2D<T>::clamp(const Vector2D<T> min, const Vector2D<T> max) const
{
	T _x = std::min(max.x, std::max(x, min.x));
	T _y = std::min(max.y, std::max(y, min.y));
	return Vector2D<T>(_x, _y);
}

template<class T>
Vector2D<T> Vector2D<T>::realiseSize(T maxDimention) const
{
	Vector2D<T> ratio( x / maxDimention, y / maxDimention);
	float maxRatio = std::max(ratio.x, ratio.y);
	return Vector2D<T>(x,y) / maxRatio;
}

template<class T>
SDL_Point Vector2D<T>::toSDLPoint()
{
	return SDL_Point{ (int)x, (int)y };
}