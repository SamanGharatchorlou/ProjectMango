#include "pch.h"
#include "Camera.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"


Camera* Camera::Get()
{
	static Camera sInstance;
	return &sInstance;
}


void Camera::clear()
{
	shakeyCam.clear();
}


Camera::Camera() : mScale(1.0f) { }


void Camera::setScale(float scale)
{
	mScale = scale;
	setViewport(rect.Size() / mScale);
}


//void Camera::setPosition(VectorF position)
//{
//	rect.SetLeftCenter(position);
//	mActiveRect = &mRect;
//}

void Camera::setViewport(VectorF viewport) 
{ 
	rect.SetSize(viewport); 
}

void Camera::follow(ECS::Entity entity)
{
	targetEntity = entity;

	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	const ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);
	VectorF position = transform.worldPosition;

	rect.SetCenter(position);

	if (rect.TopPoint() < yBoundary.x)
	{
		float diff = yBoundary.x - rect.TopPoint();
		rect.Translate( VectorF(0.0f, diff) );
	}

	if (rect.BotPoint() > yBoundary.y)
	{
		float diff = rect.BotPoint() - yBoundary.y;
		rect.Translate(VectorF(0.0f, -diff));
	}
}


void Camera::fastUpdate(float dt)
{
	VectorF translation = lerpMovement(dt);

	//rect.SetCenter(t)

//#if !CAMERA_IGNORE_BOUNDARIES
//	if( rect.LeftPoint() + translation.x >= boundaries.x1 &&
//		rect.RightPoint() + translation.x <= boundaries.x2)
//#endif
	{
		rect.Translate( VectorF(translation.x, 0.0f) );
	}


//#if !CAMERA_IGNORE_BOUNDARIES
//	if (rect.TopPoint() + translation.y >= boundaries.y1 &&
//		rect.BotPoint() + translation.y <= boundaries.y2)
//#endif
	{
		rect.Translate( VectorF(0.0f, translation.y) );
	}

	if (rect.TopPoint() < yBoundary.x)
	{
		float diff = yBoundary.x - rect.TopPoint();
		rect.Translate(VectorF(0.0f, diff));
	}

	if (rect.BotPoint() > yBoundary.y)
	{
		float diff = rect.BotPoint() - yBoundary.y;
		rect.Translate(VectorF(0.0f, -diff));
	}

	//if (shakeyCam.hasTrauma())
	//{
	//	shakeyCam.enable(rect, mBoundaries);
	//	shakeyCam.fastUpdate(dt);
	//}
}



void Camera::Update(float dt)
{
	if (shakeyCam.hasTrauma())
	{
		//mActiveRect = shakeyCam.rect();

#if PRINT_SHAKEYCAM_VALUES
		DebugPrint(Log, "Trauma %.3f | Offset = %.3f, %.3f",
			shakeyCam.trauma(),
			shakeyCam.offset().x, shakeyCam.offset().y);
#endif
	}
	else
	{
		//mActiveRect = &mRect;
	}
}


//VectorF Camera::toCameraCoords(const VectorF& worldCoords) const
//{
//	return worldCoords - rect->TopLeft();
//}


//Quad2D<float> Camera::toCameraCoords(const Quad2D<float>& worldCoords) const
//{
//	VectorF translation = toCameraCoords(worldCoords.at(0)) - worldCoords.at(0);
//	Quad2D<float> newQuad(worldCoords);
//	newQuad.translate(translation);
//
//	return newQuad;
//}


VectorF Camera::lerpMovement(float dt)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	const ECS::Transform& transform = ecs->GetComponentRef(Transform, targetEntity);
	VectorF target_position = transform.worldPosition;

	VectorF current_center = rect.Center();

	VectorF position = (target_position - current_center) * dt * 0.99;
	position.y = 0.0f;
	return position;
}