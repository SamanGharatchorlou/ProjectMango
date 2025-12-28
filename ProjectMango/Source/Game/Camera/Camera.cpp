#include "pch.h"
#include "Camera.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/SpacialComponents.h"

#define PRINT_SHAKEYCAM_VALUES 0

Camera* Camera::Get()
{
	static Camera sInstance;
	return &sInstance;
}

void Camera::setScale(float scale)
{
	mScale = scale;
	setViewport(rect.Size() / mScale);
}

void Camera::setViewport(VectorF viewport) 
{ 
	rect.SetSize(viewport); 
}

void Camera::Update(float dt)
{
	if(!ecs->IsAlive(targetEntity))
		return;

	const ECS::Transform* transform = GetComponent(Transform, targetEntity);
	if(!transform)
		return;

	const ECS::Level& level = ECS::Biome::GetLevel(targetEntity);
	
	const VectorF translation = (transform->GetObjectCenter() - rect.Center()) * dt * 5.0f;
	rect.Translate( translation );

	VectorF bounds_translation;

	// restrict within x bounds
	const float left_point = level.worldPos.x;
	const float right_point = level.worldPos.x + level.size.x;
	if(rect.LeftPoint() < left_point)
	{
		bounds_translation.x = left_point - rect.LeftPoint();
	}
	else if(rect.RightPoint() > right_point)
	{
		bounds_translation.x = right_point - rect.RightPoint();
	}

	// restrict within y bounds
	const float top_point = level.worldPos.y;
	const float bot_point = level.worldPos.y + level.size.y;
	if (rect.TopPoint() < top_point)
	{
		bounds_translation.y = top_point - rect.TopPoint();
	}
	else if (rect.BotPoint() > bot_point)
	{
		bounds_translation.y = bot_point - rect.BotPoint();
	}
	
	rect.Translate(bounds_translation);

	shakeyCam.Update(dt);
}


void Camera::InitShakeyCam(float speed, VectorF magnitude) 
{ 
	shakeyCam.speed = speed;
	shakeyCam.maxTrauma = magnitude;

	shakeyCam.x = 1.0f;
	shakeyCam.trauma = VectorF::zero();
}

RectF Camera::GetRect() const 
{ 
	RectF new_rect = rect;
	new_rect.Translate(shakeyCam.trauma);
	return new_rect;
}

void Camera::SetRect(const RectF& _rect)
{
	rect = _rect;
}

// shaky cam stuff
void Camera::AddShake(float magnitude)
{
	shakeyCam.x = 0.0f;
	
	shakeyCam.magnitude += magnitude;
	shakeyCam.magnitude = Maths::clamp(shakeyCam.magnitude, 0.0f , 1.0f);
}

void Camera::AddShake(float magnitude, VectorF source)
{
	shakeyCam.x = 0.0f;

	shakeyCam.magnitude += magnitude;
	shakeyCam.magnitude = Maths::clamp(shakeyCam.magnitude, 0.0f , 1.0f);
	
	const ECS::Level& level = ECS::Biome::GetLevel(targetEntity);
	VectorF center;
	center.x = (level.GetBounds().x1 + level.GetBounds().x2) * 0.5f;
	center.y = (level.GetBounds().y1 + level.GetBounds().y2) * 0.5f;
	if(source.x > center.x)
	{
		shakeyCam.direction.x = -1.0f;
	}
	else
	{
		shakeyCam.direction.x = 1.0f;
	}
}