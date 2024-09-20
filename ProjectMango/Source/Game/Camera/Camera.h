#pragma once

#include "CameraShake.h"


struct Camera
{
	static Camera* Get();

	void clear();

	void setScale(float scale);
	float scale() const { return mScale; }

	void setViewport(VectorF viewport);

	void setMapBoundaries(RectF boundaries) { boundaries = boundaries; }

	void Update(float dt);

	template <typename T>
	bool inView(const Rect<T>& object) const;

	template <typename T>
	bool inView(const Vector2D<T>& point) const;

	RectF GetRect() const { return rect; }
	VectorF GetSize() const { return rect.Size(); }

	void initShakeyCam(float maxTrauma, float traumaReduction) { shakeyCam.init(maxTrauma, traumaReduction); }
	CameraShake* getShake() { return &shakeyCam; }

	RectF rect;

	ECS::Entity targetEntity;

	CameraShake shakeyCam;

	float mScale;


private:
	Camera();
	~Camera() { }

	void LerpToTarget();
};

template <typename T>
bool Camera::inView(const Rect<T>& object) const
{
	// Pretend the camera is wider than it actually is as when running fast sometimes
	// there's a white flicker on the right. This would happen for any 
	// direction but given the game it mostly happens to the right
	RectF cameraRect = rect;
	cameraRect.SetWidth(cameraRect.Width() * 1.05f);

	if (object.RightPoint() < (T)cameraRect.LeftPoint()	 ||
		object.LeftPoint()	> (T)cameraRect.RightPoint() ||
		object.BotPoint()	< (T)cameraRect.TopPoint()	 ||
		object.TopPoint()	> (T)cameraRect.BotPoint())
	{
		return false;
	}
	else
		return true;
}


template <typename T>
bool Camera::inView(const Vector2D<T>& object) const
{
	if (object.x < (T)rect.LeftPoint()  ||
		object.x > (T)rect.RightPoint() ||
		object.y < (T)rect.TopPoint()   ||
		object.y > (T)rect.BotPoint()
		)
	{
		return false;
	}
	else
		return true;
}


//template <typename T>
//Rect<T> Camera::toCameraCoords(const Rect<T>& worldCoords) const
//{
//	return Rect<T>(worldCoords.TopLeft() - mActiveRect->TopLeft(), worldCoords.Size());
//}