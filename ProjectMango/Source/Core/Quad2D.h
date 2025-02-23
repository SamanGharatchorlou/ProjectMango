#pragma once

#include "Maths.h"
#include "Vector2D.h"
#include "SRect.h"

// TODO: many of these functions can be inlined, like vector
template<class T>
class Quad2D
{
public:
	using Point = Vector2D<T>;

public:
	Quad2D() : mAboutPoint(-1,-1), mRotation(0) { }
	Quad2D(Point point1, Point point2, Point point3, Point point4) : mAboutPoint(-1, -1), mRotation(0)
	{
		points = { point1, point2, point3, point4 };
	}

	Quad2D(const SRect<T>& rect) : mAboutPoint(-1, -1), mRotation(0)
	{
		points[0] = rect.TopLeft();
		points[1] = rect.TopRight();
		points[2] = rect.BotRight();
		points[3] = rect.BotLeft();
	}

	Point& operator [] (int index) { return points[index]; }
	const Point& operator [] (int index) const { return points[index]; }

	// TODO: Does it make more sense to make the about point relative to the quad i.e. point(width,height)/2 = the center. not quad.center()
	// This is how the render function works, maybe better to keep consistancy?
	// This would also enable the setRotationAboutPoint function to work properly, as in this state it wont work really.
	void resetRotation();
	void resetRotation(Point aboutpoint);
	void rotate(double degrees, Point aboutpoint);
	void rotate(double degrees);
	double rotation() const { return mRotation; }

	void setRotationAboutPoint(Point aboutPoint) { mAboutPoint = aboutPoint; }
	Point aboutPoint() const { return mAboutPoint; }

	static const int sides() { return 4; }

	T xMin() const;
	T xMax() const;
	T yMin() const;
	T yMax() const;

	Point normal0_hat() const;
	Point normal1_hat() const;

	Point normal0() const;
	Point normal1() const;

	T width() const;
	T height() const;

	void setSize(Vector2D<T> size);

	void setCenter(Point center);
	void setLeftCenter(Point leftCenter);

	void translate(Vector2D<T> movement);

	Point center() const;
	Point rightCenter() const;
	Point topCenter() const;
	Point botCenter() const;
	Point leftCenter() const;

	SRect<T> getRect() const;


private:
	Point points[4];

	Point mAboutPoint;
	double mRotation;
};


// typedefs
typedef Quad2D<float> QuadF;


template<class T>
SRect<T> Quad2D<T>::getRect() const
{
	SRect<T> rect;
	rect.SetSize(width(), height());
	rect.SetCenter(center());
	return rect;
}


template<class T>
void Quad2D<T>::setSize(Vector2D<T> size) // relative to point0
{
	Vector2D<T> widthDirection = (points[1] - points[0]).normalise();
	Vector2D<T> heightDirection = (points[3] - points[0]).normalise();

	// point0 doesn't move

	// set point1
	points[1] = points[0] + size.x * widthDirection;

	// set point2
	points[2] = points[1] + size.y * heightDirection;

	// set point3
	points[3] = points[2] + size.x * widthDirection;
}


template<class T>
T Quad2D<T>::width() const
{
	return Vector2D<T>::distance(points[0], points[1]);
}

template<class T>
T Quad2D<T>::height() const
{
	return Vector2D<T>::distance(points[1], points[2]);
}


template<class T>
void Quad2D<T>::setLeftCenter(Point newLeftCenter)
{
	Point currentLeftCenter = leftCenter();
	Vector2D<T> offset = newLeftCenter - currentLeftCenter;
	translate(offset);
}


template<class T>
void Quad2D<T>::setCenter(Point newCenter)
{
	Point currentCenter = center();
	Vector2D<T> offset = newCenter - currentCenter;
	translate(offset);
}


template<class T>
void Quad2D<T>::translate(Vector2D<T> movement)
{
	for (int i = 0; i < sides(); i++)
	{
		points[i] = points[i] + movement;
	}
}


template<class T>
Vector2D<T> Quad2D<T>::center() const
{
	Point sum;
	for (int i = 0; i < sides(); i++)
	{
		sum += points[i];
	}

	return sum / (T)sides();
}


template<class T>
Vector2D<T> Quad2D<T>::rightCenter() const
{
	return (points[1] + points[2]) / 2.0;
}


template<class T>
Vector2D<T> Quad2D<T>::leftCenter() const
{
	return (points[3] + points[0]) / 2.0;
}


template<class T>
Vector2D<T> Quad2D<T>::topCenter() const
{
	return (points[0] + points[1]) / 2.0;
}

template<class T>
Vector2D<T> Quad2D<T>::botCenter() const
{
	return (points[2] + points[3]) / 2.0;
}



template<class T>
Vector2D<T> Quad2D<T>::normal0_hat() const
{
	float dx = points[1].x - points[0].x;
	float dy = points[1].y - points[0].y;

	return VectorF(dy, -dx).normalise();
}


template<class T>
Vector2D<T> Quad2D<T>::normal1_hat() const
{
	float dx = points[2].x - points[1].x;
	float dy = points[2].y - points[1].y;

	return VectorF(dy, -dx).normalise();
}


template<class T>
Vector2D<T> Quad2D<T>::normal0() const
{
	float dx = points[1].x - points[0].x;
	float dy = points[1].y - points[0].y;

	return VectorF(dy, -dx);
}


template<class T>
Vector2D<T> Quad2D<T>::normal1() const
{
	float dx = points[2].x - points[1].x;
	float dy = points[2].y - points[1].y;

	return VectorF(dy, -dx);
}


template<class T>
void Quad2D<T>::rotate(double degrees, Point about_point)
{
	mRotation += degrees;
	while(mRotation > 360)
		mRotation -= 360;

	double cosine = cos(Maths::toRadians(degrees));
	double sine = sin(Maths::toRadians(degrees));

	for (int i = 0; i < sides(); i++)
	{
		points[i] = points[i].rotateVector(sine, cosine, about_point);
	}
}


template<class T>
void Quad2D<T>::rotate(double degrees)
{
#if DEBUG_MODE
	if (mAboutPoint.isNegative())
	{
		DebugPrint(Warning, "Attempting to rotate quad %f degrees, without an about point set.\
			Use setRotationAboutPoint() first or provide an about point", mRotation);
	}
#endif

	mRotation += degrees;
	while (mRotation > 360)
		mRotation -= 360;

	double cosine = cos(Maths::toRadians(degrees));
	double sine = sin(Maths::toRadians(degrees));

	for (int i = 0; i < sides(); i++)
	{
		points[i] = rotateVector(points[i], mAboutPoint, sine, cosine);
	}
}


template<class T>
void Quad2D<T>::resetRotation(Point aboutpoint)
{
	rotate(-mRotation, aboutpoint);
	ASSERT(mRotation == 0, "Rotation is not 0 after rotation has been reset, rotation %f", mRotation);
}


template<class T>
void Quad2D<T>::resetRotation()
{
#if DEBUG_MODE
	if (mAboutPoint.isNegative())
	{
		DebugPrint(Warning, "Attempting to reset rotation of quad without an about point set.\
			Use setRotationAboutPoint() first or provide an about point");
	}
#endif

	rotate(-mRotation, mAboutPoint);
	ASSERT(mRotation == 0, "Rotation is not 0 after rotation has been reset, rotation %f", mRotation);
}


template<class T>
T Quad2D<T>::xMin() const
{
	T minX = points[0].x;
	for (int i = 0; i < sides(); i++)
	{
		if (minX > points[i].x)
			minX = points[i].x;
	}

	return minX;
}


template<class T>
T Quad2D<T>::xMax() const
{
	T maxX = points[0].x;
	for (int i = 0; i < sides(); i++)
	{
		if (maxX < points[i].x)
			maxX = points[i].x;
	}

	return maxX;
}


template<class T>
T Quad2D<T>::yMin() const
{
	T minY = points[0].y;
	for (int i = 0; i < sides(); i++)
	{
		if (minY > points[i].y)
			minY = points[i].y;
	}

	return minY;
}


template<class T>
T Quad2D<T>::yMax() const
{
	T maxY = points[0].y;
	for (int i = 0; i < sides(); i++)
	{
		if (maxY < points[i].y)
			maxY = points[i].y;
	}

	return maxY;
}
