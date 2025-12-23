#pragma once

#include "CameraShake.h"

struct Camera
{
	static Camera* Get();

	void setViewport(VectorF viewport);
	void setScale(float scale);
	float getScale() const { return mScale; }

	void setMapBoundaries(RectF boundaries) { boundaries = boundaries; }

	void Update(float dt);

	void SetRect(const RectF& rect);
	RectF GetRect() const;
	VectorF GetSize() const { return rect.Size(); }
	
	void InitShakeyCam(float speed, VectorF magnitude);
	void AddShake(float magnitude);
	void AddShake(float magnitude, VectorF source);
	CameraShake* getShake() { return &shakeyCam; }
	
	ECS::Entity targetEntity;
	CameraShake shakeyCam;
	

private:
	RectF rect;
	float mScale = 1.0f;
};